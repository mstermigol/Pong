#include "../headers/log.h"
#include "../headers/config.h"
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

void logMessage(const char *format, ...)
{
    time_t current_time;
    char *time_string;

    current_time = time(NULL);
    time_string = ctime(&current_time);
    time_string[strlen(time_string) - 1] = '\0';

    va_list args;
    va_start(args, format);

    FILE *logFile = fopen(LOG_FILE, "a");
    if (logFile != NULL)
    {
        fprintf(logFile, "[%s] ", time_string);
        vfprintf(logFile, format, args);
        fprintf(logFile, "\n");
        fclose(logFile);
    }
    else
    {
        perror("Error opening log file");
    }

    va_end(args);
}