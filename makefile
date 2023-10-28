.PRECIOUS: object/%.o object/test_%.o
BIN_DIR = binary
INC_DIR = include
LIB_DIR = lib
OBJ_DIR = object
SRC_DIR = source
DEP_DIR = dependencies
TEST_DIR = tests

LIB_TABLE_R = $(addprefix $(OBJ_DIR)/,data.o entry.o list.o table.o)
TABLE_CLIENT_R = $(addprefix $(OBJ_DIR)/,data.o \
	entry.o \
	table_client.o \
	sdmessage.pb-c.o \
	message.o\
	client_stub.o \
	network_client.o)
TABLE_SERVER_R = $(addprefix $(OBJ_DIR)/,data.o \
	entry.o \
	list.o \
	table.o \
	sdmessage.pb-c.o \
	message.o\
	table_server.o \
	table_skel.o \
	network_server.o)

# CFLAGS = -Wall -Werror -g -MMD -MP -MF -I $(INC_DIR)

CC = gcc
CFLAGS = -Wall -Werror -g -I $(INC_DIR)
ARCHIVE = ar -rcs
PROTO_LIB = -I/usr/local/include -L/usr/local/lib -lprotobuf-c

all: libtable table_client table_server

libtable: $(LIB_TABLE_R)
	$(ARCHIVE) $(LIB_DIR)/$@.a $^

table_client: $(TABLE_CLIENT_R)
	$(CC) $^ $(PROTO_LIB) $(LIB_DIR)/libtable.a -o $(BIN_DIR)/$@

table_server: $(TABLE_SERVER_R)
	$(CC) $^ $(PROTO_LIB) $(LIB_DIR)/libtable.a -o $(BIN_DIR)/$@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Include makefiles from dependencies
include $(wildcard $(DEP_DIR)/*.d)

clean:
	rm $(OBJ_DIR)/*.o $(LIB_DIR)/*.a $(BIN_DIR)/*
