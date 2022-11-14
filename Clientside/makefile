CC=gcc

run_client: client.o
	$(CC) -o run_client client.o

client.o: client.c
	$(CC) -c client.c
	
clean:
	rm *.o run_client
