CC = gcc
CFLAGS = -Wall -Werror -g
TARGET = aesdsocket
OBJ = aesdsocket.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET) $(OBJ)

