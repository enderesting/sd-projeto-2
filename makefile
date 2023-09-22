BIN_DIR = bin
INC_DIR = include
LIB_DIR = lib
OBJ_DIR = obj
SRC_DIR = src
DEP_DIR = dependencies

CC = gcc
CFLAGS = -Wall -Werror -g -MMD -MP -MF $(DEP_DIR)/$*.d -I $(INC_DIR)

EXECS = $(BIN_DIR)/test

make: $(EXECS)

$(BIN_DIR)/test: $(OBJ_DIR)/test.o $(OBJ_DIR)/aluno.o
    $(CC) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
    $(CC) $(CFLAGS) -c $< -o $@

# Include makefiles from dependencies
include $(wildcard $(DEP_DIR)/*.d)

clean:
    rm -rf $(DEP_DIR)/* $(OBJ_DIR)/* $(BIN_DIR)/*
