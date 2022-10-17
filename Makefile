# Makefile, versao 1
# Redes de Computadores 2021_22, grupo 12

CC     = g++
# -Wall  - this flag is used to turn on most compiler warnings
CFLAGS = -Wall

.PHONY: all clean

all: user DS

user: Client/User.cpp utils.o
	$(CC) $(CFLAGS) -o user Client/User.cpp utils.o

utils.o: utils.cpp constant.hpp
	$(CC) $(CFLAGS) -c -o utils.o utils.cpp

DS: Server/Server.cpp utils.o
	$(CC) $(CFLAGS) -o DS Server/Server.cpp utils.o
	
clean:
	rm -f DS user *.o
