/*
* Copyright (C) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
* Distributed under the terms of the GPL v3 license, available in the file LICENSE.
*/

#include "logger.hpp"

int8_t Logger::init(const char* source = ""){
    return set_source(this->_msg, source);
}

void Logger::log_info(const char* text) {
    char buffer[90];
    sprintf(buffer,  "LOG_INFO: ");
    strcat(buffer, text);
    set_text(this->_msg, buffer);

    publish();
}
void Logger::log_warn(const char* text) {
    char buffer[90];
    sprintf(buffer,  "WARN: ");
    strcat(buffer, text);
    set_text(this->_msg, buffer);

    publish();
}
void Logger::log_error(const char* text) {
    char buffer[90];
    sprintf(buffer,  "ERROR: ");
    strcat(buffer, text);
    set_text(this->_msg, buffer);

    publish();
}

void Logger::publish(){
    dronecan_protocol_debug_log_message_publish(&this->_msg, &_transfer_id);
    _transfer_id++;
}

#ifdef _DEBUG
void Logger::log_debug(const char* text) {
    char buffer[90];
    sprintf(buffer,  "DEBUG: ");
    strcat(buffer, text);
    set_text(this->_msg, buffer);
    
    publish();
}
#else
void Logger::log_debug(const char* text) {
    // Do nothing if _DEBUG is not defined
}
#endif