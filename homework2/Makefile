install: installServer installClient

installServer: utils.o server.o
	cc -o myFTserver utils.o server.o

installClient: utils.o client.o
	cc -o myFTclient utils.o client.o

server.o: server/server.c server/server.h
	cc -c server/server.c

client.o: client/client.c client/client.h
	cc -c client/client.c

utils.o: utils/utils.c utils/utils.h
	cc -c utils/utils.c
