CC := gcc
CFLAGS := -std=c23 -pthread -g3 \
		  -Wall -Wextra -Wno-unused-parameter -Werror=return-type \
		  -fsanitize=undefined -fsanitize-recover=undefined \
		  -MMD -MP
LDLIBS := -lc
LDFLAGS := -fsanitize=undefined -fsanitize-recover=undefined

TARGET_EXEC := a.out

SRCS := $(shell find -name '*.c' -not -name '*_tst.c')
OBJS := $(SRCS:.c=.o)
DEPS := $(OBJS:.o=.d)

TEST_SRCS := $(shell find  -name '*_tst.c')
TEST_EXECS := $(TEST_SRCS:.c=.out)

ifeq ($(asan), 1)
CFLAGS += -fsanitize=address -fsanitize-recover=address
LDFLAGS += -fsanitize=address -fsanitize-recover=address
endif


$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDLIBS) $(LDFLAGS)


%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@


%_tst.out: %_tst.c
	$(CC) $(CFLAGS) $< -o $@


.PHONY: clean tests
clean:
	rm *.o *.d $(TEST_EXECS) $(TARGET_EXEC)

tests: $(TEST_EXECS)

-include $(DEPS)
