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

// static inline bool format_str(uint8_t &dest, const char *fmt, va_list arg) {
//     char buffer[90];
//     sprintf(buffer, fmt, arg);
//     return buffer;
// }

static inline bool set_text(DebugLogMessage_t &msg, const char *str) {
    if (strlen(str) > 90) {
    return false;
    }

    // TODO: test if char and unsigned char are casted correctly
    strcpy((char*) msg.text, str);
    msg.text_size = strlen(str);
    return true;
}