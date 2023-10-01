#TODO Change folder names into their extended versions, in the directory and in the makefile
BIN_DIR = bin
INC_DIR = include
LIB_DIR = lib
OBJ_DIR = obj
SRC_DIR = src
DEP_DIR = dependencies

CC = gcc
CFLAGS = -Wall -Werror -g -MMD -MP -MF $(DEP_DIR)/$*.d -I $(INC_DIR)

EXECS = $(BIN_DIR)/test_data $(BIN_DIR)/test_entry $(BIN_DIR)/test_list

make: $(EXECS)

$(BIN_DIR)/test_data: $(OBJ_DIR)/data.o
	$(CC) $^ -o $@

$(BIN_DIR)/test_entry: $(OBJ_DIR)/data.o $(OBJ_DIR)/entry.o
	$(CC) $^ -o $@

$(BIN_DIR)/test_list: $(OBJ_DIR)/data.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/list.o
	$(CC) $^ -o $@        

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Include makefiles from dependencies
include $(wildcard $(DEP_DIR)/*.d)

clean:
	rm -f -i $(OBJ_DIR)/*.o $(BIN_DIR)/test
