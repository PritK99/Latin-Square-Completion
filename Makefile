CC = g++

all: bfs dfs bestfs plits_seq plits_par

bfs: src/bfs.cpp include/lsc.hpp
	$(CC) -o bfs src/bfs.cpp -I include/

dfs: src/dfs.cpp include/lsc.hpp
	$(CC) -o dfs src/dfs.cpp -I include/

bestfs: src/bestfs.cpp include/lsc.hpp
	$(CC) -o bestfs src/bestfs.cpp -I include/

plits_seq: src/plits_seq.cpp include/plits.hpp
	$(CC) -o plits_seq src/plits_seq.cpp -I include/

plits_par: src/plits_par.cpp include/plits.hpp
	$(CC) -o plits_par src/plits_par.cpp -fopenmp -I include/

mis: src/plits_par.cpp include/plits.hpp
	$(CC) -o mis_par src/lsc-mis.cpp -fopenmp

list:
	@echo "List of Targets (Algorithms): "
	@echo "- bfs (Breadth First Search)"
	@echo "- dfs (Depth First Search)"
	@echo "- bestfs (Best First Search)"
	@echo "- plits_seq (Partial Legal & Illegal Tabu Search)"
	@echo "- plits_par (Parallelised Partial Legal & Illegal Tabu Search)"

clean:
	rm -f bfs dfs bestfs plits_seq plits_par mis
