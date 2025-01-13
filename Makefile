CC = gcc
CFLAGS = -Wall -g

# default target
all: $(SERVER) $(CLIENT)

client: client.c
	$(CC) $(CFLAGS) client.c -o client

server: server.c
	$(CC) $(CFLAGS) server.c -o server

# clean up generated files
clean:
	rm -f $(OBJS) $(TARGET)