# No folders template makefile

INCLUDES := ./
CC       := gcc
CFLAGS   := -Werror -Wextra -Wpedantic

SOURCES   := $(shell find ./ -type f -name '*.c')
LIBS      :=
OBJS      := $(patsubst %.c,%.o,$(SOURCES))
OBJS_LIBS := $(patsubst %.c,%.o,$(LIBS))
TARGET := example

all: $(TARGET)

$(TARGET): $(OBJS) $(OBJS_LIBS)
	$(CC) $(CFLAGS) $(OBJS) $(OBJS_LIBS) -o $(TARGET) $(foreach inc, $(INCLUDES), -I $(inc))

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@ $(foreach inc, $(INCLUDES), -I $(inc))

clean:
	rm -f $(TARGET) $(OBJS) $(OBJS_LIBS)
