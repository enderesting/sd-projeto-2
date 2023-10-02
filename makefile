BIN_DIR = bin
INC_DIR = include
LIB_DIR = lib
OBJ_DIR = obj
SRC_DIR = src
DEP_DIR = dependencies
TEST_DIR = tests

TARGETS = data entry list serialization table
EXECS = $(foreach target,$(TARGETS),$(BIN_DIR)/test_$(target))

CC = gcc
CFLAGS = -Wall -Werror -g -MMD -MP -MF -I $(INC_DIR)
# CFLAGS = -g -I $(INC_DIR)

compile: $(EXECS)

$(BIN_DIR)/test_%: $(OBJ_DIR)/test_%.o $(OBJ_DIR)/%.o
	$(CC) $^ -o $@

$(OBJ_DIR)/test_%.o: $(SRC_DIR)/test_%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Include makefiles from dependencies
include $(wildcard $(DEP_DIR)/*.d)

clean:
	rm $(OBJ_DIR)/*.o $(BIN_DIR)/test_*
