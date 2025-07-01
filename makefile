SRC_DIR  := ./src/main
HDR_DIR  := ./src/headers
RSC_DIR  := ./src/resources
LIBS_DIR := ./libs
OUT_DIR  := ./out
BIN_DIR  := $(OUT_DIR)/bin

ALL_DIRS := $(SRC_DIR) $(HDR_DIR) $(RSC_DIR) $(OUT_DIR) $(BIN_DIR) $(LIBS_DIR)
TARGET   := $(BIN_DIR)/cpm

INCLUDES := $(HDR_DIR) ./libs/cJSON/include/
CC       := gcc
CFLAGS   := -Werror -Wall -Wextra -Wpedantic
LFLAGS   := 

SOURCES   := $(shell find $(SRC_DIR) -type f -name '*.c')
LIBS      := $(shell find $(LIBS_DIR) -type f -name '*.c')
OBJS      := $(patsubst $(SRC_DIR)/%.c,$(OUT_DIR)/%.o,$(SOURCES))
OBJS_LIBS := $(patsubst $(LIBS_DIR)/%.c,$(OUT_DIR)/%.o,$(LIBS))

all: $(TARGET) resources

$(TARGET): $(OBJS) $(OBJS_LIBS) | $(ALL_DIRS)
	$(CC) $(CFLAGS) $(OBJS) $(OBJS_LIBS) -o $(TARGET) $(foreach inc, $(INCLUDES), -I $(inc)) $(LFLAGS)

$(OUT_DIR)/%.o:: $(SRC_DIR)/%.c
	mkdir -p $(@D)/ && $(CC) $(CFLAGS) -c $^ -o $@ $(foreach inc, $(INCLUDES), -I $(inc))

$(OUT_DIR)/%.o:: $(LIBS_DIR)/%.c
	mkdir -p $(@D)/ && $(CC) $(CFLAGS) -c $^ -o $@ $(foreach inc, $(INCLUDES), -I $(inc))

$(ALL_DIRS):
	@mkdir -p $@

resources:
	@cp -rf $(RSC_DIR)/ $(BIN_DIR)/

test:
	@echo $(SOURCES)

clean:
	rm -r $(OUT_DIR)/*
