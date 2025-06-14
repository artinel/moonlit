SHELL = bash
CC = gcc
CFLAGS = -g -Wall -Wextra -O3 $(shell pkg-config --cflags --libs libadwaita-1 sqlite3 sdl2) -I./src/include -lSDL2_mixer
GRESOURCE_CONFIG = gresource.xml
GRESOURCE_TARGET = src/resources.c
TARGET = moonlit
DOC-CONFIG = doc-config

CSOURCES = $(shell find src -type f -name "*.c")

all: compile

compile-resources:
	glib-compile-resources $(GRESOURCE_CONFIG) --target $(GRESOURCE_TARGET) --generate-source

compile: compile-resources
	$(CC) $(CFLAGS) $(CSOURCES) -o moonlit

clean:
	rm $(TARGET)

run:
	./$(TARGET)

docs:
	doxygen $(DOC-CONFIG)

install:
	cp $(TARGET) /usr/bin
uninstall:
	rm /usr/bin/$(TARGET)
