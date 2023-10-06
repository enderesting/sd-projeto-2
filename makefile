.PRECIOUS: obj/%.o obj/test_%.o
BIN_DIR = bin
INC_DIR = include
LIB_DIR = lib
OBJ_DIR = obj
SRC_DIR = src
DEP_DIR = dependencies
TEST_DIR = tests

TARGETS = data entry list serialization table
EXECS = $(foreach target,$(TARGETS),$(BIN_DIR)/test_$(target))
OBJS = $(foreach target,$(TARGETS),$(OBJ_DIR)/test_$(target).o)

bin/test_data :=
bin/test_entry := obj/data.o
bin/test_list := obj/data.o obj/entry.o
bin/test_serialization := obj/data.o obj/entry.o obj/list.o
bin/test_table := obj/data.o obj/list.o obj/entry.o


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
