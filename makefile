.PRECIOUS: object/%.o object/test_%.o
BIN_DIR = binary
INC_DIR = include
LIB_DIR = library
OBJ_DIR = object
SRC_DIR = source
DEP_DIR = dependencies
TEST_DIR = tests

TARGETS = data entry list serialization table
EXECS = $(foreach target,$(TARGETS),$(BIN_DIR)/test_$(target))
OBJS = $(foreach target,$(TARGETS),$(OBJ_DIR)/test_$(target).o)

binary/test_data :=
binary/test_entry := object/data.o
binary/test_list := object/data.o object/entry.o
binary/test_serialization := object/data.o object/entry.o object/list.o
binary/test_table := object/data.o object/list.o object/entry.o


CC = gcc
# CFLAGS = -Wall -Werror -g -MMD -MP -MF -I $(INC_DIR)
CFLAGS = -Wall -Werror -g -I $(INC_DIR)

compile: $(EXECS)

$(BIN_DIR)/test_%: $($@) $(OBJ_DIR)/test_%.o $(OBJ_DIR)/%.o
	$(CC) $^ $($@) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/test_%.o: $(SRC_DIR)/test_%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Include makefiles from dependencies
include $(wildcard $(DEP_DIR)/*.d)

clean:
	rm $(OBJ_DIR)/*.o $(BIN_DIR)/test_*
