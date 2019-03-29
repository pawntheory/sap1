CC=gcc
CFLAGS=-O2 -Wall -Wextra -Isrc -DNDEBUG $(OPTFLAGS)

PROJ=build/sap1

SOURCES=$(wildcard src/**/*.c src/*.c)
HEADERS=$(wildcard src/**/*.h src/*.h)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

all: $(OBJECTS) $(PROJ)

$(PROJ): build $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

build:
	@mkdir -p build

debug: CFLAGS=-g -O2 -Wall -Wextra -Isrc $(OPTFLAGS)
debug: all

.PHONY: clean
clean:
	rm -rf build $(OBJECTS)
	rm -f $(PROJ)
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`
