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
	client_stub.o \
	network_client.o)
TABLE_SERVER_R = $(addprefix $(OBJ_DIR)/,data.o \
	entry.o \
	list.o \
	table.o \
	table_server.o \
	network_server.o)

CC = gcc
# CFLAGS = -Wall -Werror -g -MMD -MP -MF -I $(INC_DIR)
CFLAGS = -Wall -Werror -g -I $(INC_DIR)
ARCHIVE = ar -rcs

all: libtable table-client table-server

libtable: $(LIB_TABLE_R)
	$(ARCHIVE) $(OBJ_DIR)/$@.a $^

table-client: $(TABLE_CLIENT_R)
	$(CC) $^ libtable.a -o $@

table-server: $(TABLE_SERVER_R)
	$(CC) $^ libtable.a -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Include makefiles from dependencies
include $(wildcard $(DEP_DIR)/*.d)

clean:
	rm $(OBJ_DIR)/*.o $(BIN_DIR)/test_*
