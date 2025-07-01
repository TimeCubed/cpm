# Minimal template makefile

# Change these to your liking!
SRC_DIR  := ./src
OUT_DIR  := ./out
BIN_DIR  := $(OUT_DIR)/bin

ALL_DIRS := $(SRC_DIR) $(OUT_DIR) $(BIN_DIR)
TARGET   := $(BIN_DIR)/example

# Add any include folders here!
# This template assumes you have headers in $(SRC_DIR)
INCLUDES := $(SRC_DIR)
CC       := gcc
CFLAGS   := -Werror -Wextra -Wpedantic

# These will automatically find all .c source files for you
# No need to add them manually, though you can if this breaks somehow
SOURCES   := $(shell find $(SRC_DIR) -type f -name *.c)
LIBS      :=
# These will be used for automatic dependency checking
OBJS      := $(patsubst %.c,$(OUT_DIR)/%.o,$(notdir $(SOURCES)))
OBJS_LIBS := $(patsubst %.c,$(OUT_DIR)/%.o,$(notdir $(LIBS)))

all: $(TARGET)

$(TARGET): $(OBJS) $(OBJS_LIBS) | $(ALL_DIRS)
	$(CC) $(CFLAGS) $(OBJS) $(OBJS_LIBS) -o $(TARGET) $(foreach inc, $(INCLUDES), -I $(inc))

$(OUT_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)/ && $(CC) $(CFLAGS) -c $^ -o $@ $(foreach inc, $(INCLUDES), -I $(inc))

$(OBJS_LIBS):
	$(foreach lib, $(LIBS), $(CC) $(CFLAGS) -c $(lib) -o $(patsubst %.c,$(OUT_DIR)/%.o,$(lib)))

$(ALL_DIRS):
	@mkdir -p $@

clean:
	rm -f $(TARGET) $(OBJS)
