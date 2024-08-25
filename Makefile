#based on https://makefiletutorial.com/#makefile-cookbook

#vars
CC := gcc
STDFLAGS := -std=c17 -pthread -g
WFLAGS := -Wall -Wextra -Wno-unused-parameter -Werror=return-type
LDLIBS := -lc

#config
TARGET_EXEC := a.out

BUILD_DIR := ./build
SRC_DIRS := ./src
INC_DIRS := $(shell find $(SRC_DIRS) -type d)


#build
SRCS := $(shell find $(SRC_DIRS) -name '*.c')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS := $(STDFLAGS) $(WFLAGS) $(INC_FLAGS) -MMD -MP

#steps

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDLIBS) $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

-include $(DEPS)
