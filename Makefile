CC = gcc
CFLAGS = -Wall -std=c99 -I./src
LIBS = -lnfc -lcurl -lcjson -lssl -lcrypto

SOURCES = src/main.c src/nfc.c src/api.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = synquid

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
