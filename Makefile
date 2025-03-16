SHELL = bash
CC = gcc
CFLAGS = -Wall -Wextra -Werror -O3 $(shell pkg-config --cflags --libs libadwaita-1) -I./src/include
GRESOURCE_CONFIG = gresource.xml
GRESOURCE_TARGET = src/resources.c
TARGET = moonlit

CSOURCES = $(shell find src -type f -name "*.c")

all: compile

compile-resources:
	glib-compile-resources $(GRESOURCE_CONFIG) --target $(GRESOURCE_TARGET) --generate-source

compile: compile-resources
	$(CC) $(CFLAGS) $(CSOURCES) -o moonlit
