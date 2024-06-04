install: finger.o parser.o
	cc -o finger finger.o parser.o

parser.o: parser.c parser.h
	cc -c parser.c

finger.o: finger.c finger.h
	cc -c finger.c
