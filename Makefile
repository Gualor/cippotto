TARGET := chippotto

CC := gcc
MKDIR := mkdir -p
RMDIR := rm -rf

SRC_DIR := src
INC_DIR := inc
BIN_DIR := bin

SRCS := $(shell find $(SRC_DIR) -name '*.c')
INCS := $(addprefix -I, $(shell find $(INC_DIR) -type d))
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.o,$(SRCS))
DEPS := $(patsubst %.o,%.d,$(OBJS))

CFLAGS := -g $(INCS) -MMD -MP -Wall -Wextra -Wdouble-promotion
LDFLAGS := 

.PHONY: all
all: $(BIN_DIR)/$(TARGET)

.PHONY: clean
clean:
	$(RMDIR) $(BIN_DIR)

$(BIN_DIR)/$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) $< -o $@ -c

-include $(DEPS)
