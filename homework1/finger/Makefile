install: finger.o parser.o printer.o
	cc -o finger finger.o parser.o printer.o

parser.o: parser.c parser.h
	cc -c parser.c

printer.o: printer.c printer.h
	cc -c printer.c

finger.o: finger.c finger.h
	cc -c finger.c
