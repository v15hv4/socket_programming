IDIR=include
SDIR=src

CC=g++
CFLAGS=-I$(IDIR)
LIBS=-pthread

COMMON_DEPS=utils.cpp

server: server.cpp $(COMMON_DEPS)
	$(CC) $(CFLAGS) $(LIBS) $(COMMON_DEPS) server.cpp -o server
	chmod +x ./server

client: client.cpp $(COMMON_DEPS)
	$(CC) $(CFLAGS) $(LIBS) $(COMMON_DEPS) client.cpp -o client
	chmod +x client

clean:
	rm server client
