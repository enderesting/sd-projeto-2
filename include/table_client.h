#ifndef _TABLE_CLIENT_H
#define _TABLE_CLIENT_H

#include <stdlib.h>
#include "entry.h"
#include "table_client-private.h"
#include "client_stub.h"

#define PUT_STR "put"
#define GET_STR "get"
#define DEL_STR "del"
#define SIZE_STR "size\n"
#define GETKEYS_STR "getkeys\n"
#define GETTABLE_STR "gettable\n"
#define QUIT_STR "quit\n"

typedef enum { PUT, GET, DEL, SIZE, GETKEYS, GETTABLE, QUIT, INVALID } operation;

typedef enum { ENTRY, KEY, NONE } content;

operation parse_operation(char* op_str);

content parse_content(char* c_str);

#endif
