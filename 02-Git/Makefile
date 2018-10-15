PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
TARGET=Rock–paper–scissors

OBJS = main.o

ifeq ($(BUILD_MODE),debug)
	CFLAGS += -g
else ifeq ($(BUILD_MODE),run)
	CFLAGS += -O2
else
	$(error Build mode $(BUILD_MODE) not supported by this Makefile)
endif

all:	$(TARGET)

$(TARGET):	$(OBJS)
	$(CC) -o $@ $^

%.o:	$(PROJECT_ROOT)%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	rm -fr $(TARGET) $(OBJS)
