#include "http.h"

static int callback(void *data, int args_num, char **argv, char **argc) {
    for(int i = 0; i < args_num; i++) {
        cout << argc[i] << " = " << (argv[i] ? argv[i] : "NULL") << "\t";
    }
    cout << endl;
    return 0;
}

string sqlpass;
static int callback_select_user(void *data, int args_num, char **argv, char **argc) {
    sqlpass = string(argv[0]);
    return 0;
}

static int callback_get_number(void *data, int args_num, char **argv, char **argc) {
    int *n = (int *) data;
    *n = args_num;
    return 0;
}

int count = 0;
static int callback_get_like_acount(void *data, int args_num, char **argv, char **argc) {
    count = atoi(argv[0]);
    for(int i =0;i<args_num;i++){ 
        cout << "select " << argv[i] << endl;
    }
    return 0;

}

int get_ipv4_socket_bind_and_listen(struct sockaddr_in *server_address, int port, int backlog) {
    int server_fd;
    server_address->sin_family = AF_INET;//set socket domain.
    server_address->sin_port = htons(port);//host bytes-order to network bytes-order.
    server_address->sin_addr.s_addr = INADDR_ANY;// ip address is 0.0.0.0

    //establish a socket.
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("establish socket failed");
        exit(EXIT_FAILURE);//exit(1)
    }
    cout << "server socket: " << server_fd << endl;
    //bind server socket to server address.
    if((bind(server_fd, (struct sockaddr *)server_address, sizeof(struct sockaddr))) < 0) {
        perror("socket bind failed");
        exit(EXIT_FAILURE);
    }
    cout << "socket bind successful." << endl;
    //listen to connection from clients.
    if(listen(server_fd, backlog) < 0) {
        perror("start listening failed");
        exit(EXIT_FAILURE);
    }
    cout << "listening port: " << port << endl;
    return server_fd;
}

string get_request_file_name(string buf) {
    //the request buf is like "GET /index.html HTTP/1.1 ... ..."
    int buf_index = 0;
    string info;
    while(buf_index < buf.size() && buf[buf_index++] != '/') info += buf[buf_index - 1];
    if(buf_index - 1 == buf.size() || buf[buf_index - 1] != '/')return "";//illegal request.
    string file_name;
    //extract request file from the request buf.
    while(buf[buf_index++] != ' ') file_name += buf[buf_index - 1];
    cout << info + file_name << endl;
    return file_name;
}

void handle_get(int client_sockfd, string file_name) {
    struct stat st;
    int f;
    //if file not find then return 404.
    if(stat(file_name.c_str(), &st) < 0) {
        handle_get(client_sockfd, path + "/404.html");
        return;
    }
    //open request file.
    if((f = open(file_name.c_str(), O_RDONLY)) < 0) { //BUG!! I writed ") < 0)){"
        perror("open file failed");
        close(client_sockfd);
        return;
    }
    //send headers.
    write(client_sockfd, HEADER.c_str(), HEADER.size());
    //send file to client.
    sendfile(client_sockfd, f, NULL, st.st_size + 1);
    close(f);

    close(client_sockfd);//must close.
}

string get_time() {
    time_t t = time(nullptr);
    //asctime():transform date and time to broken-down time or ASCII.
    char *time = asctime(localtime(&t));
    string res(time);
    if(res[int(res.size()) - 1] == '\n')
        res = res.substr(0, int(res.size()) - 1);
    return res;
}

void handle_post(int client_sockfd, string buf) {
    int buf_index = 0;
    while(buf[buf_index++] != '/');
    string method;
    while(buf[buf_index++] != ' ') method += buf[buf_index - 1];
    cout << method << endl;
    sqlite3 *db;
    char *errMsg;
    //string sql = "insert into user values ('wc','wc',1);";
    int rc = sqlite3_open(db_file.c_str(), &db);
    if(rc != SQLITE_OK) {
        cout << "open sqlite3 failed." << endl;
    }
    //int rs = sqlite3_exec(db,sql.c_str(),0,0,&errMsg);
    //sql = "select password from user where username='wc'";
    int num;
    //sqlite3_exec(db,sql.c_str(),callback,(void *)&first,&errMsg);
    try {
        if(method == "login") {
            buf_index = buf.find("username");
            string state = "failed";
            if(buf_index != -1) {
                pair<string, string> username_password = get_username_password(buf.substr(buf_index, buf.size() - buf_index));
                string username = username_password.first;
                string password = username_password.second;
                cout << "(username: " << username << "  password: " << password << ")" << endl;
                string sql = "select password from user where username='" + username + "'";
                cout << sql << endl;
                sqlpass = "-1";
                sqlite3_exec(db, sql.c_str(), callback_select_user, 0, &errMsg);
                cout << errMsg << endl;
                cout << "sqlpass: " << sqlpass << endl;
                if (sqlpass == password) {
                    state = username;
                }

                struct stat info;
                string folder = dir_path + "/" + username;
                if(!(stat(folder.c_str(), &info) == 0 && (info.st_mode & S_IFDIR))) {
                    if(mkdir(folder.c_str(), 0777) == -1) {
                        cout << "create folder failed." << endl;
                    }
                }
            }
            write(client_sockfd, HEADER.c_str(), HEADER.size());
            send(client_sockfd, state.c_str(), state.size(), 0);
        } else if(method == "register") {
            buf_index = buf.find("username");
            string state = "failed";
            if(buf_index != -1) {
                pair<string, string> username_password = get_username_password(buf.substr(buf_index, buf.size() - buf_index));
                string username = username_password.first;
                string password = username_password.second;
                cout << "(username: " << username << "  password: " << password << ")" << endl;
                string sql = "select password from user where username='" + username + "'";
                sqlite3_exec(db, sql.c_str(), callback_get_number, (int *)&num, &errMsg);
                cout << "number: " << num << endl;
                if(num == 0) {
                    string sql = "insert into user values ('" + username + "','" + password + "',1);";
                    int rs = sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
                    cout << errMsg << endl;
                    state = "successed";
                }
            }
            write(client_sockfd, HEADER.c_str(), HEADER.size());
            send(client_sockfd, state.c_str(), state.size(), 0);
        } else if(method == "upvote") {
            string state = "failed";
            buf_index = buf.find("filename");
            if(buf_index != -1) {
                pair<string, string> filename_index = get_filename_index1(buf.substr(buf_index, buf.size() - buf_index));
                string filename = filename_index.first;
                string index = filename_index.second;
                filename = url_decode(filename);
                if(filename.back() == '/')filename.pop_back();
                if(index == "true") {
                    string sql = "update files set like_acount = like_acount + 1 where filename = '" + filename + "';";
                    cout << sql << endl;
                    int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
                    if(rc == SQLITE_OK) state = "successed";
                } else {
                    string sql = "update files set like_acount = like_acount - 1 where filename = '" + filename + "';";
                    cout << sql << endl;
                    int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
                    if(rc == SQLITE_OK) state = "successed";
                }
            }
            write(client_sockfd, HEADER.c_str(), HEADER.size());
            send(client_sockfd, state.c_str(), state.size(), 0);
        } else if(method == "downvote") {
            string state = "failed";
            buf_index = buf.find("filename");
            if(buf_index != -1) {
                pair<string, string> filename_index = get_filename_index2(buf.substr(buf_index, buf.size() - buf_index));
                string filename = filename_index.first;
                string index = filename_index.second;
                filename = url_decode(filename);
                if(filename.back() == '/')filename.pop_back();
                if(index == "true") {
                    string sql = "update files set dislike_acount = dislike_acount + 1 where filename = '" + filename + "';";
                    cout << sql << endl;
                    int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
                    if(rc == SQLITE_OK)state = "successed";
                } else {
                    string sql = "update files set dislike_acount = dislike_acount - 1 where filename = '" + filename + "';";
                    cout << sql << endl;
                    int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
                    if(rc == SQLITE_OK)state = "successed";
                }
                write(client_sockfd, HEADER.c_str(), HEADER.size());
                send(client_sockfd, state.c_str(), state.size(), 0);
            }
        } else if(method == "upload") {
            string state = "success";
            buf_index = buf.find("start=");
            vector<string> res = handle_upload(buf.substr(buf_index, buf.size() - buf_index));
            string start = res[0];
            string file_name = res[1];
            string path = res[2] + file_name;
            string folder = res[2];
            string file_data = res[3];
            path = url_decode(path);
            file_data = url_decode(file_data);
            folder = url_decode(folder);
            fstream fs;
            if(mkdir(folder.c_str(), 0777) == -1) {
                cout << "create folder failed." << endl;
            }
            ifstream ifile(path.c_str());
            if(ifile) {
                state = "file already exists.";
            } else {
                fs.open(path.c_str(), fstream::in | fstream::out | fstream::trunc);
                //ofstream out(path.c_str());
                if(fs) {
                    fs << file_data;
                    fs.close();
                    cout << "save to " << path << endl;
                    string sql = "insert into files values ('" + path + "',0,0);";
                    printf("%s", sql.c_str());
                    sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
                } else {
                    state = "create file failed.";
                }
            }

            write(client_sockfd, HEADER.c_str(), HEADER.size());
            send(client_sockfd, state.c_str(), state.size(), 0);
        } else if(method == "delete") {
            string state = "success";
            buf_index = buf.find("start=");
            vector<string> res = handle_upload(buf.substr(buf_index, buf.size() - buf_index));
            string file_name = res[1];
            string path = res[2] + file_name;
            string folder = res[2];
            path = url_decode(path);
            folder = url_decode(folder);
            if(remove(path.c_str()) != 0) {
                state = "delete " + file_name + " failed";
            }
            if(state != "success") {
                string command = "rm -rf " + path;
                int rc = system(command.c_str());
                if(rc == 0) {
                    state = "success";
                    string sql = "delete from files where filename=" + path;
                    printf("%s", sql.c_str());
                    sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
                }
            }

            write(client_sockfd, HEADER.c_str(), HEADER.size());
            send(client_sockfd, state.c_str(), state.size(), 0);
        } else if(method == "newfolder") {
            string state = "success";
            buf_index = buf.find("start=");
            vector<string> res = handle_upload(buf.substr(buf_index, buf.size() - buf_index));
            string folder = res[2] + res[1];
            folder = url_decode(folder);
            struct stat info;
            if(stat(folder.c_str(), &info) == 0 && (info.st_mode & S_IFDIR)) {
                state = "folder already exists.";
            } else {
                if(mkdir(folder.c_str(), 0777) == -1) {
                    cout << "create folder failed." << endl;
                }else{ 
                    string sql = "insert into files values ('" + folder + "',0,0);";
                    sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
                }
            }

            write(client_sockfd, HEADER.c_str(), HEADER.size());
            send(client_sockfd, state.c_str(), state.size(), 0);
        }
        else if(method == "getByUrl") {
            buf_index = buf.find("start=");
            vector<string> res = handle_upload(buf.substr(buf_index, buf.size() - buf_index));
            string folder = res[2];
            folder = url_decode(folder);
            string result;
            result = get_files_by_url(folder, db, num, errMsg);
            write(client_sockfd, HEADER.c_str(), HEADER.size());
            send(client_sockfd, result.c_str(), result.size(), 0);
        }
    } catch(...) {
        cout << "post error..." << endl;
    }
    sqlite3_close(db);
    close(client_sockfd);
}

vector<string> handle_upload(string s) {
    int start_index = 6;
    int file_name_index = s.find("file_name=");
    int path_index = s.find("path=");
    int file_data_index = s.find("file_data=");
    vector<string> res(4);
    res[0] = s.substr(start_index, file_name_index - start_index - 1);
    file_name_index += 10;
    res[1] = s.substr(file_name_index, path_index - file_name_index - 1);
    path_index += 5;
    res[2] = s.substr(path_index, file_data_index - path_index - 1);
    file_data_index += 10;
    res[3] = s.substr(file_data_index, s.size() - file_data_index);
    return res;
}

pair<string, string> get_filename_index1(string s) {
    int filename_index = 9;
    int index_upvote = s.find("index_upvote");
    pair<string, string>res("", "");
    res.first = s.substr(filename_index, index_upvote - filename_index - 1);
    index_upvote += 13;
    res.second = s.substr(index_upvote, s.size() - index_upvote);
    return res;
}

pair<string, string> get_filename_index2(string s) {
    int filename_index = 9;
    int index_downvote = s.find("index_downvote");
    pair<string, string>res("", "");
    res.first = s.substr(filename_index, index_downvote - filename_index - 1);
    index_downvote += 15;
    res.second = s.substr(index_downvote, s.size() - index_downvote);
    return res;
}

pair<string, string> get_username_password(string s) {
    int username_index = 9;
    int password_index = s.find("password");
    pair<string, string>res("", "");
    res.first = s.substr(username_index, password_index - username_index - 1);
    password_index += 9;
    res.second = s.substr(password_index, s.size() - password_index);
    return res;
}

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *socket_thread(void *arg) {
    //pthread_mutex_lock(&lock);
    char request_buf[N];
    int client_sockfd = *((int *)arg);
    //get GET request from client.
    memset(request_buf, '\0', sizeof(char) * N);
    read(client_sockfd, request_buf, N - 1);//last is '\0'.
    string buf(request_buf);
    //cout << buf << endl;
    //cout << "****" << buf << "*****" <<endl;
    cout << "[" << get_time() << "] ";
    string method;
    for(int buf_index = 0; buf_index < buf.size(); buf_index++) {
        if(buf[buf_index] == '/')break;
        method += buf[buf_index];
    }
    if(method.substr(0, 4) == "POST") {
        cout <<  method;
        handle_post(client_sockfd, buf);
        pthread_exit(NULL);
    }
    string file_name = get_request_file_name(buf);
    if(file_name.substr(0, 4) != "dir/") {
        file_name = path + file_name;
    }
    if(file_name == "") {
        pthread_exit(NULL);
    }
    // cout << file_name << endl;
    //handle request: send file.
    handle_get(client_sockfd, file_name);
    //pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
}

unsigned char to_hex(unsigned char x) {
    return  x > 9 ? x + 55 : x + 48;
}

unsigned char from_hex(unsigned char x) {
    unsigned char y;
    if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
    else if (x >= '0' && x <= '9') y = x - '0';
    else assert(0);
    return y;
}

string url_encode(const string &str) {
    string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++) {
        if (isalnum((unsigned char)str[i]) ||
                (str[i] == '-') ||
                (str[i] == '_') ||
                (str[i] == '.') ||
                (str[i] == '~'))
            strTemp += str[i];
        else if (str[i] == ' ')
            strTemp += "+";
        else {
            strTemp += '%';
            strTemp += to_hex((unsigned char)str[i] >> 4);
            strTemp += to_hex((unsigned char)str[i] % 16);
        }
    }
    return strTemp;
}

string url_decode(const string &str) {
    string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++) {
        if (str[i] == '+') strTemp += ' ';
        else if (str[i] == '%') {
            assert(i + 2 < length);
            unsigned char high = from_hex((unsigned char)str[++i]);
            unsigned char low = from_hex((unsigned char)str[++i]);
            strTemp += high * 16 + low;
        } else strTemp += str[i];
    }
    return strTemp;
}

void get_file_by_name(string path, vector<string> &filenames) {
    DIR *pDir;
    struct dirent *ptr;
    if(!(pDir = opendir(path.c_str()))) {
        std::cout << "Folder doesn't Exist!" << std::endl;
        return;
    }
    while((ptr = readdir(pDir)) != 0) {
        string info;
        if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0) {
            //info = path + '/' + ptr->d_name + " tpye: " + (char*)ptr->d_type;
            //cout<<ptr->d_name<<"   "<<ptr->d_type<<endl;
            filenames.push_back(path + ptr->d_name);
        }
    }
    closedir(pDir);
}

string byte_to_size(long size) {
    stringstream stream;
    string result;
    if(size > 0 && size < 1024) {
        stream << size;
        stream >> result;
        return result + "B";
    } else if (size > 1023 && size < 1048576) {
        stream << (size / 1024);
        stream >> result;
        return result + "KB";
    } else if (size > 1048576 && size < 1073741824) {
        stream << (size / 1048576);
        stream >> result;
        return result + "MB";
    } else if (size > 1073741824 && size < 1099511627776) {
        stream << (size / 1073741824);
        stream >> result;
        return result + "GB";
    }
    return "0";
}
string second_to_time(time_t time) {
    struct tm *p;
    p = gmtime(&time);
    char s[100];
    strftime(s, sizeof(s), "%Y-%m-%d %H-%M-%S", p);
    return s;
}
//返回当前路径下的字节数
string file_size2(const char *filename) {

    string time;
    struct stat s;
    long size = 0;
    string result;
    if (stat(filename, &s) == 0) {
        if(s.st_mode & S_IFDIR) {
            //std::cout<<"it's a directory"<<std::endl;
            time = second_to_time(s.st_mtime);
            result = result + "1:-1:" + time;
        } else if (s.st_mode & S_IFREG) {
            //std::cout<<"it's a file"<<std::endl;
            size = s.st_size;
            string file_size = byte_to_size(size);
            time = second_to_time(s.st_mtime);
            result = result + "0:" + file_size + ":" + time;
        } else {
            //std::cout<<"not file not directory"<<std::endl;

        }
    } else {
        //std::cout<<"error, doesn't exist"<<std::endl;
    }

    return result;
}

string get_files_by_url(string url, sqlite3 *db, int num, char *errMsg) {
    vector<string> filenames;
    string result;
    get_file_by_name(url, filenames);
    for(int i = 0 ; i < filenames.size() ; i++) {
        vector<int> like = get_likes(filenames[i], db, num, errMsg);
        string temp = result + filenames[i] + ":" + file_size2(filenames[i].c_str()) + ":" + to_string(like[0]) + ":" + to_string(like[1]) + "?";
        result += temp;
    }
    return result;
}


vector<int> get_likes(string filename, sqlite3 *db, int num, char *errMsg){ 
    count = 0;
    string sql = "select like_acount from files where filename = '" + filename + "';";
    cout << sql << endl;
    sqlite3_exec(db, sql.c_str(), callback_get_like_acount, 0, &errMsg);
    vector<int>res;
    res.push_back(count);
    cout << "like count:" << count << endl;
    sql = "select dislike_acount from files where filename = '" + filename + "';";
    cout << sql << endl;
    sqlite3_exec(db, sql.c_str(), callback_get_like_acount, 0, &errMsg);
    res.push_back(count);
    return res;
}
