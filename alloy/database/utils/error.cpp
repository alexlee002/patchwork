//
//  error.cpp
//  aldb-test
//
//  Created by Alex Lee on 11/08/2017.
//  Copyright © 2017 Alex Lee. All rights reserved.
//

#include "error.hpp"
#include <libgen.h>

namespace aldb {

Error::Error(const std::string &domain, int64_t code, const char *message)
    : domain(domain), code(code), message(message){};

Error::Error(const Error &other) : domain(other.domain), code(other.code), message(other.message) {}

Error::operator std::string() const {
    std::string msg;
    msg.append("[" + domain + ": " + std::to_string(code) + "] " + message);
    if (!file.empty()) {
        msg.append("; First occur in:\"");
        msg.append(std::string(basename((char *)file.c_str())) + ": "+ std::to_string(line) +"\"");
    }
    msg.append("\n");
    return msg;
}

const std::string Error::description() const { return std::string(*this); }

}