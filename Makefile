TARGET := cippotto

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

CFLAGS := $(INCS) -MMD -MP -Os -ffunction-sections -fdata-sections -Wl,--gc-sections \
          -Wall -Wextra -Wdouble-promotion -fno-strict-aliasing -fno-exceptions

LDFLAGS := -lm -lraylib
ifeq ($(OS), Windows_NT)
    LDFLAGS += -lopengl32 -lgdi32 -lwinmm
else ifeq ($(shell uname -s), Linux)
    LDFLAGS += -lGL -lpthread -ldl -lrt -lX11
else
    $(error Platform $(OS) currently not supported)
endif

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
