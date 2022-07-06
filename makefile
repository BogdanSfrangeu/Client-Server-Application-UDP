server.o : server.c
	gcc -c server.c
server : server.o
	gcc -o server server.o
	./server &

client.o : client.c
	gcc -c client.c
client : client.o
	gcc -o client client.o
	#./client 127.0.0.1 (-s|-z|-m|-a)

clean :
	rm -f *.o
	killall -9 server
	rm -f server
	rm -f client
	clear
