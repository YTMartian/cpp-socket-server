//
// Created by tim on 2021/9/28.
//

#include "ProcessorFactory.h"

ProcessorFactory::ProcessorFactory() = default;

ProcessorFactory::~ProcessorFactory() = default;

Processor *ProcessorFactory::get_processor(llhttp_method method) {
    switch (method) {
        case llhttp_method::HTTP_GET:
            return new GetProcessor();
        case llhttp_method::HTTP_POST:
            return new PostProcessor();
        default:
            return nullptr;
    }
}