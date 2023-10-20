CC = gcc
CFLAGS = -Wall -Wextra

TARGET = Bluebird

SRC = ./src

SRCS = $(wildcard $(SRC)/*.c)

all : $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

.PHONY : clean
clean:
	-rm Bluebird