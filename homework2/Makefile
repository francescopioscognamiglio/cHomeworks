install: installServer installClient

installServer: utils.o connection.o sender.o receiver.o server-parser.o server.o
	cc -o myFTserver utils.o connection.o sender.o receiver.o server-parser.o server.o

installClient: utils.o connection.o sender.o receiver.o client-parser.o client.o
	cc -o myFTclient utils.o connection.o sender.o receiver.o client-parser.o client.o

server.o: server/server.c server/server.h
	cc -c server/server.c

client.o: client/client.c client/client.h
	cc -c client/client.c

utils.o: utils/utils.c utils/utils.h
	cc -c utils/utils.c

connection.o: connection/connection.c connection/connection.h
	cc -c connection/connection.c

sender.o: sender/sender.c sender/sender.h
	cc -c sender/sender.c

receiver.o: receiver/receiver.c receiver/receiver.h
	cc -c receiver/receiver.c

server-parser.o: server/parser.c server/parser.h
	cc -c server/parser.c -o server-parser.o

client-parser.o: client/parser.c client/parser.h
	cc -c client/parser.c -o client-parser.o
