/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */

#ifndef _TABLE_SKEL_PRIVATE_H
#define _TABLE_SKEL_PRIVATE_H

#include <sys/time.h>
#include <time.h>
#include "sdmessage.pb-c.h"

MessageT* respond_bad_op(MessageT* msg);
MessageT* respond_err_in_exec(MessageT* msg);

int return_time(struct timeval* tv);

#endif
