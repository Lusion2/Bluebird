CC = gcc
CFLAGS = -Wall -Wextra
LFLAGS = -Wl,-subsystem,windows -mwindows -ld2d1 -ldwrite -lkernel32 -luser32 -ldxguid -lcomctl32 -luuid -lopengl32

TARGET = Bluebird

SRC = ./src

SRCS = $(wildcard $(SRC)/*.c)

all : $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) $(LIBS) -o $(TARGET) $(SRCS) $(LFLAGS)

.PHONY : clean
clean:
	-rm Bluebird