all: Server Client

Client: mainC.o Snake.o
	g++ -o Client mainC.o Snake.o -pthread -lncurses

mainC.o: /tmp/tmp.ypdfQ9pZi7/main.cpp
	g++ -c /tmp/tmp.ypdfQ9pZi7/main.cpp -o mainC.o

Snake.o: /tmp/tmp.ypdfQ9pZi7/Snake.cpp /tmp/tmp.ypdfQ9pZi7/Snake.h
	g++ -c /tmp/tmp.ypdfQ9pZi7/Snake.cpp -o Snake.o

clean:
	rm -f *.o Server Client

# Server
Server: mainS.o Gamer.o
	g++ -o Server mainS.o Gamer.o -pthread

mainS.o: /tmp/tmp.PnwJoR55DT/main.cpp
	g++ -c /tmp/tmp.PnwJoR55DT/main.cpp -o mainS.o

Gamer.o: /tmp/tmp.PnwJoR55DT/Gamer.cpp /tmp/tmp.PnwJoR55DT/Gamer.h
	g++ -c /tmp/tmp.PnwJoR55DT/Gamer.cpp -o Gamer.o