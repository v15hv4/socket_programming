IDIR=include
SDIR=src

CC=g++
CFLAGS=-I$(IDIR)
LIBS=-pthread

_DEPS=utils dictdb
DEPS=$(patsubst %, %.cpp, $(_DEPS))

server: server.cpp $(DEPS)
	$(CC) $(CFLAGS) $(LIBS) $(DEPS) server.cpp -o server
	chmod +x ./server

client: client.cpp $(COMMON_DEPS)
	$(CC) $(CFLAGS) $(LIBS) $(DEPS) client.cpp -o client
	chmod +x client

clean:
	rm server client
