# Extended template makefile

# Change these to your liking!
SRC_DIR  := ./src/main
HDR_DIR  := ./src/headers
RSC_DIR  := ./src/resources
# Add any library source files in here, and they'll automatically
# be compiled!
LIBS_DIR := ./libs
OUT_DIR  := ./out
BIN_DIR  := $(OUT_DIR)/bin

ALL_DIRS := $(SRC_DIR) $(HDR_DIR) $(RSC_DIR) $(OUT_DIR) $(BIN_DIR)
TARGET   := $(BIN_DIR)/example

# Add any include folders here!
# Make sure to include your library includes too!
INCLUDES := $(HDR_DIR)
CC       := gcc
CFLAGS   := -Werror -Wextra -Wpedantic
LFLAGS   :=

# These will automatically find all .c source files for you
# No need to add them manually, though you can if this breaks somehow
SOURCES   := $(shell find $(SRC_DIR) -type f -name '*.c')
LIBS      := $(shell find $(LIBS_DIR) -type f -name '*.c')
# These will be used for automatic dependency checking
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

clean:
	rm -f $(TARGET) $(OBJS) $(OBJS_LIBS)
	rm -r $(BIN_DIR)/resources
