/* Grupo 50
 * Filipe Costa - 55549
 * Yichen Cao - 58165
 * Emily Sá - 58200
 * Github repo: https://github.com/padrezulmiro/sd-projeto/
 */

#ifndef _TABLE_SKEL_PRIVATE_H
#define _TABLE_SKEL_PRIVATE_H

#include "sdmessage.pb-c.h"
MessageT* respond_bad_op(MessageT* msg);
MessageT* respond_err_in_exec(MessageT* msg);


#endif
