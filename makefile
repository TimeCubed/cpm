ROOT_DIR := ./src
OUT_DIR  := ./out
BIN_DIR  := $(OUT_DIR)/bin

# Main SRC
MAIN_DIR := $(ROOT_DIR)/main
SRC_DIR  := $(MAIN_DIR)/c
HDR_DIR  := $(MAIN_DIR)/headers

INCLUDES := $(HDR_DIR)
SOURCES  := $(shell find $(SRC_DIR)/ -type f -name '*.c')
OBJS     := $(patsubst $(SRC_DIR)/%.c,$(OUT_DIR)/%.o,$(SOURCES))

# Resources
RSC_DIR  := $(ROOT_DIR)/resources

# Libraries
LIBS_DIR := ./libs

LIBS      := $(shell find $(LIBS_DIR) -type f -name '*.c')
OBJS_LIBS := $(patsubst $(LIBS_DIR)/%.c,$(OUT_DIR)/%.o,$(LIBS))

ALL_DIRS := $(ROOT_DIR) $(OUT_DIR) $(BIN_DIR) $(MAIN_DIR) $(SRC_DIR) $(HDR_DIR) $(RSC_DIR) $(LIBS_DIR)
TARGET   := $(BIN_DIR)/cpm

CC       := gcc
CFLAGS   := -Werror -Wall -Wextra -Wpedantic
CPPFLAGS := 
LFLAGS   := 

all: linux | resources

$(TARGET): $(OBJS) $(OBJS_LIBS) | $(ALL_DIRS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OBJS) $(OBJS_LIBS) -o $(TARGET) $(foreach inc, $(INCLUDES), -I $(inc)) $(LFLAGS)

$(OUT_DIR)/%.o:: $(SRC_DIR)/%.c
	mkdir -p $(@D)/ && $(CC) $(CFLAGS) $(CPPFLAGS) -c $^ -o $@ $(foreach inc, $(INCLUDES), -I $(inc))

$(OUT_DIR)/%.o:: $(LIBS_DIR)/%.c
	mkdir -p $(@D)/ && $(CC) $(CFLAGS) $(CPPFLAGS) -c $^ -o $@ $(foreach inc, $(INCLUDES), -I $(inc))

$(ALL_DIRS):
	@mkdir -p $@

analysis: CFLAGS += -fanalyzer
analysis: linux $(TARGET) | resources

debug: CFLAGS += -g -O0
debug: linux $(TARGET) | resources

linux: CC := gcc
linux: CPPFLAGS += -D LINUX
linux: $(TARGET) | resources

windows: CC := x86_64-w64-mingw32-gcc
windows: CPPFLAGS += -D WINDOWS
windows: $(TARGET) | resources

resources:
	@cp -rf $(RSC_DIR)/ $(BIN_DIR)/

clean:
	rm -r $(OUT_DIR)/*
