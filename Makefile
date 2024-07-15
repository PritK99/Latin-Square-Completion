CC = g++

all: bfs dfs bestfs plits_seq plits_par

bfs: src/bfs.cpp include/lsc.hpp
	$(CC) -o bfs src/bfs.cpp -I include/

dfs: src/dfs.cpp include/lsc.hpp
	$(CC) -o dfs src/dfs.cpp -I include/

bestfs: src/bestfs.cpp include/lsc.hpp
	$(CC) -o bestfs src/bestfs.cpp -I include/

plits_seq: src/plits.cpp
	$(CC) -o plits_seq src/plits.cpp

plits_par: src/plits.cpp
	$(CC) -o plits_par src/plits.cpp -fopenmp

clean:
	rm -f bfs dfs bestfs plits_seq plits_par
