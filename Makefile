CC = gcc
CFLAGS = -g -Wall

server:
	$(CC) $(CFLAGS) server.c -o backup_server 
client:
	$(CC) $(CFLAGS) client.c -o backup_client
clean_server:
	rm backup_server 
clean_client:
	rm backup_client
