CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic -pthread -lrt
OBJFILES = ./libs/main.o ./libs/args_handler.o ./libs/err_handler.o
TARGET = proj2

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES)
	rm -f $(OBJFILES)
