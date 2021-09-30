//
// Created by tim on 2021/9/28.
//

#ifndef SERVER_PROCESSORFACTORY_H
#define SERVER_PROCESSORFACTORY_H

#include "head.h"
#include "Processor.h"
#include "GetProcessor.h"
#include "PostProcessor.h"

class ProcessorFactory {
public:
    ProcessorFactory();

    ~ProcessorFactory();

    static Processor *get_processor(llhttp_method method);
};


#endif //SERVER_PROCESSORFACTORY_H
