CC = gcc
CFLAGS = -Wall -g

client: client.c
	$(CC) $(CFLAGS) client.c -o client

rs: rs.c
	$(CC) $(CFLAGS) rs.c -o rs

# clean up generated files
clean:
	rm -f %.o rs client