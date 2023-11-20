#  * Grupo 50
#  * Filipe Costa - 55549
#  * Yichen Cao - 58165
#  * Emily Sá - 58200
#  * Github repo: https://github.com/padrezulmiro/sd-projeto/
.PRECIOUS: object/%.o object/test_%.o
BIN_DIR = binary
INC_DIR = include
LIB_DIR = lib
OBJ_DIR = object
SRC_DIR = source
DEP_DIR = dependencies
TEST_DIR = tests

$(shell mkdir -p $(BIN_DIR) >/dev/null)
$(shell mkdir -p $(DEP_DIR) >/dev/null)
$(shell mkdir -p $(OBJ_DIR) >/dev/null)
$(shell mkdir -p $(LIB_DIR) >/dev/null)

LIB_TABLE_R = $(addprefix $(OBJ_DIR)/,data.o entry.o list.o table.o) 
TABLE_CLIENT_R = $(addprefix $(OBJ_DIR)/,data.o \
	entry.o \
	mutex.o \
	table_client.o \
	sdmessage.pb-c.o \
	message.o\
	client_stub.o \
	network_client.o \
	stats.o)
TABLE_SERVER_R = $(addprefix $(OBJ_DIR)/,data.o \
	entry.o \
	mutex.o \
	list.o \
	table.o \
	sdmessage.pb-c.o \
	message.o\
	table_server.o \
	table_skel.o \
	network_server.o \
	server_thread.o \
	stats.o)
# CFLAGS = -Wall -Werror -g -MMD -MP -MF -I $(INC_DIR)

CC = gcc
CFLAGS = -Wall -Werror -g -MMD -MP -I $(INC_DIR) -pthread
ARCHIVE = ar -rcs
PROTO_LIB = -I/usr/local/include -L/usr/local/lib -lprotobuf-c

all: $(LIB_DIR)/libtable.a $(addprefix $(BIN_DIR)/,table_client \
												table_server)

$(SRC_DIR)/sdmessage.pb-c.c: sdmessage.proto
	protoc-c --c_out=. sdmessage.proto
	mv sdmessage.pb-c.c $(SRC_DIR)
	mv sdmessage.pb-c.h $(INC_DIR)

$(LIB_DIR)/libtable.a: $(LIB_TABLE_R)
	$(ARCHIVE) $@ $^

$(BIN_DIR)/table_client: $(TABLE_CLIENT_R)
	$(CC) $^ $(PROTO_LIB) $(LIB_DIR)/libtable.a -o $@

$(BIN_DIR)/table_server: $(TABLE_SERVER_R)
	$(CC) $^ $(PROTO_LIB) $(LIB_DIR)/libtable.a -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -MF $(DEP_DIR)/$*.d -c $< -o $@ 

# Include makefiles from dependencies
include $(wildcard $(DEP_DIR)/*.d)

clean:
	rm $(OBJ_DIR)/*.o $(DEP_DIR)/*.d $(LIB_DIR)/*.a $(BIN_DIR)/*
