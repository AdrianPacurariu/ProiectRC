CC=gcc

run_server: server.o
	$(CC) -o run_server server.o

server.o: server.c
	$(CC) -c server.c -pthread
	
clean:
	rm *.o run_server
