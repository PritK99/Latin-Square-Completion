#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <random>
#include <algorithm>

using namespace std;

class Graph {
    public:
    int n;
    map<pair<int, int>, set<pair<int, int>>> adj_list;

    void add_edge(pair<int, int> n1, pair<int, int> n2) {
        adj_list[n1].insert(n2);
        adj_list[n2].insert(n1);
    }

    void print_graph() {
        for (auto& x: adj_list) {
            cout << "(" << x.first.first << ", " << x.first.second << ")"<< ": ";
            for (auto y: x.second) {
                cout << "(" << y.first << ", " << y.second << ")" << " ";
            }
            cout << "\r\n";
        }
    }
}; 

class LSC : public Graph {
    public:
    vector<vector<int>> square;
    // Domain set
    map<pair<int, int>, set<int>> D; 
    // Color classes
    map<int, set<pair<int, int>>> V; 

    LSC() {
        ;
    }

    LSC(vector<vector<int>> partial) {
        int x = partial.size();
        vector<pair<int, int>> Cand_set;
        n = x * x;
        square = partial;
        for (int i=0; i<x; i++) {
            for (int j=0; j<x; j++) {
                for (int k=j+1; k<x; k++) {
                    add_edge({i, j}, {i, k});
                    add_edge({j, i}, {k, i});
                }
                Cand_set.push_back({i, j});
                for (int k=0; k<x; k++) {
                    D[{i, j}].insert(k+1);
                }
            }
        }
        // Randomly assign color sets
        shuffle(Cand_set.begin(), Cand_set.end(), default_random_engine(time(0)));
        srand(time(0));
        for (auto& It: Cand_set) {
            int r = rand() % D[It].size();
            auto Iter = D[It].begin();
            advance(Iter, r);
            int color = *Iter;
            V[color+1].insert(It);
            D[It].erase(Iter);
            square[It.first][It.second] = color;
        }
    } 

    void printSquare() {
        int x = square.size();
        for (int i=0; i < x; i++) {
            for (int j=0; j < x; j++) {
                cout << square[i][j] << " " ;
            }
            cout << "\n";
        }
    }
};

int main() {
    LSC test(
        {{-1, -1, -1},
         {-1, -1, -1},
         {-1, -1, -1}
        }
    );
    // test.print_graph();
    test.printSquare();
    return 0;
}