#include "global_logger.h"

struct logger o_o;

int log_init(const char* prefix, unsigned flags, unsigned int max_megabytes)
{
    return logger_init(&o_o, prefix, flags, max_megabytes);
}
