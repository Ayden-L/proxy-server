CC = gcc
CFLAGS = -Wall -g

# target executable
TARGET = client

# src files
SRCS = client.c proxy.c server.c

# object files
OBJS = client.o proxy.o server.o

# default target
all: $(TARGET)

# build the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# build the object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# clean up generated files
clean:
	rm -f $(OBJS) $(TARGET)