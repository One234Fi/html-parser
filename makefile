CC := gcc
CFLAGS := -std=c23 -pthread -g3 \
		  -Wall -Wextra -Wno-unused-parameter -Werror=return-type \
		  -fsanitize=undefined -fsanitize-recover=undefined

LDLIBS := -lc
LDFLAGS := -fsanitize=undefined -fsanitize-recover=undefined



TARGET_EXEC := a.out
TEST_EXEC := test.out

BUILD_DIR := ./build
SRC_DIRS := ./src
INC_DIRS := $(shell find $(SRC_DIRS) -type d)

TEST_DIRS := $(SRC_DIRS) ./test
INC_TEST_DIRS := $(shell find $(TEST_DIRS) -type d)



SRCS := $(shell find $(SRC_DIRS) -name '*.c')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

TEST_SRCS := $(shell find $(TEST_DIRS) -name '*.c' -not -name 'main.c')
TEST_OBJS := $(TEST_SRCS:%=$(BUILD_DIR)/%.o)
TEST_DEPS := $(TEST_OBJS:.o=.d)

INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS += $(INC_FLAGS) -MMD -MP 

ifeq ($(asan), 1)
CFLAGS += -fsanitize=address -fsanitize-recover=address
LDFLAGS += -fsanitize=address -fsanitize-recover=address
endif



$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDLIBS) $(LDFLAGS)


$(BUILD_DIR)/$(TEST_EXEC): $(TEST_OBJS)
	$(CC) $(TEST_OBJS) -o $@ $(LDLIBS) $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@



.PHONY: clean test run
clean:
	rm -r $(BUILD_DIR)

test: $(BUILD_DIR)/$(TEST_EXEC)
	$(BUILD_DIR)/$(TEST_EXEC)

run: $(BUILD_DIR)/$(TARGET_EXEC)
	$(BUILD_DIR)/$(TARGET_EXEC)

-include $(DEPS)
