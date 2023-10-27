.PRECIOUS: object/%.o object/test_%.o
BIN_DIR = binary
INC_DIR = include
LIB_DIR = library
OBJ_DIR = object
SRC_DIR = source
DEP_DIR = dependencies
TEST_DIR = tests

LIB_TABLE_R = $(addprefix $(OBJ_DIR)/,data.o entry.o list.o table.o)
TABLE_CLIENT_R = $(addprefix $(OBJ_DIR)/,data.o \
	entry.o \
	table_client.o \
	sdmessage.pb-c.o \
	client_stub.o \
	network_client.o)
TABLE_SERVER_R = $(addprefix $(OBJ_DIR)/,data.o \
	entry.o \
	list.o \
	table.o \
	sdmessage.pb-c.o \
	table_server.o \
	table_skel.o \
	network_server.o)

CC = gcc
# CFLAGS = -Wall -Werror -g -MMD -MP -MF -I $(INC_DIR)
CFLAGS = -Wall -Werror -g -I $(INC_DIR)  
ARCHIVE = ar -rcs

all: libtable table_client table_server

libtable: $(LIB_TABLE_R)
	$(ARCHIVE) $(OBJ_DIR)/$@.a $^

table_client: $(TABLE_CLIENT_R)
	$(CC) $^ -I/usr/local/include -L/usr/local/lib -lprotobuf-c $(OBJ_DIR)/libtable.a -o $@

table_server: $(TABLE_SERVER_R)
	$(CC) $^ -I/usr/local/include -L/usr/local/lib -lprotobuf-c $(OBJ_DIR)/libtable.a -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Include makefiles from dependencies
include $(wildcard $(DEP_DIR)/*.d)

clean:
	rm $(OBJ_DIR)/*.o $(BIN_DIR)/test_*
