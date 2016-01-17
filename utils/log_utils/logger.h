//
// Created by alvaregd on 27/11/15.
//

#ifndef SIMPLE_LEVEL_EDITOR_LOGGER_H
#define SIMPLE_LEVEL_EDITOR_LOGGER_H

#define GL_LOG_FILE "./build/gl.log"
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>

bool restart_gl_log();
bool gl_log (const char* message,...);
bool gl_log_err(const char* message, ...);

#endif //SIMPLE_LEVEL_EDITOR_LOGGER_H
