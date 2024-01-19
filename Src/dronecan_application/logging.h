#ifndef SRC_LOGGING_H_
#define SRC_LOGGING_H_


#include "uavcan/protocol/debug/LogMessage.h"
#include <stdarg.h>


static inline bool set_source(DebugLogMessage_t &msg, const char *str) {
    if (strlen(str) > 31) {
        return false;
    }

    // TODO: test if char and unsigned char are casted correctly
    strcpy((char*) msg.source, str);
    msg.source_size = strlen(str);
    return true;
}

static inline bool set_text(DebugLogMessage_t &msg, const char *str) {
    if (strlen(str) > 90) {
    return false;
    }

    // TODO: test if char and unsigned char are casted correctly
    strcpy((char*) msg.text, str);
    msg.text_size = strlen(str);
    return true;
}
#endif // SRC_LOGGING_H_