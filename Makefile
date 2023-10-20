CC = gcc
CFLAGS = -Wall -Wextra
LFLAGS = -Wl,-subsystem,windows -mwindows

TARGET = Bluebird

SRC = ./src

SRCS = $(wildcard $(SRC)/*.c)

all : $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LFLAGS)

.PHONY : clean
clean:
	-rm Bluebird