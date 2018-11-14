OBJS     = bankserver.o utils.o hashtable.o lista.o trlista.o 
SOURCE   = bankserver.cpp utils.cpp hashtable.cpp lista.cpp trlista.cpp 
HEADER   = utils.hpp hashtable.hpp lista.hpp trlista.hpp 
OUT      = bankserver
OBJS2     = bankclient.o
SOURCE2   = bankclient.cpp 
OUT2      = bankclient
CC       = g++
FLAGS    = -g3 -c -lsocket -lnsl -pthread -lpthread


all: $(OUT) $(OUT2)

$(OUT): $(OBJS)
	$(CC) -o $@ $(OBJS) -g3 -lpthread

$(OUT2): $(OBJS2)
	$(CC) -o $@ $(OBJS2) -g3 -lpthread

bankserver.o: bankserver.cpp
	$(CC) $(FLAGS) bankserver.cpp

utils.o: utils.cpp
	$(CC) $(FLAGS) utils.cpp

hashtable.o: hashtable.cpp
	$(CC) $(FLAGS) hashtable.cpp

lista.o: lista.cpp
	$(CC) $(FLAGS) lista.cpp

trlista.o: trlista.cpp
	$(CC) $(FLAGS) trlista.cpp

bankclient.o: bankclient.cpp
	$(CC) $(FLAGS) bankclient.cpp

clean:
	rm -f *.o $(OUT) $(OUT2)