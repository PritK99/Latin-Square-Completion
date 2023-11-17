#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <random>
#include <algorithm>

using namespace std;

template <typename T>
bool find(vector<T>& v, T e) {
    for (auto& It: v) {
        if (It == e) return true;
    }
    return false;
}

class Graph {
    public:
    int n;
    map<pair<int, int>, vector<pair<int, int>>> adj_list;

    void add_edge(pair<int, int> n1, pair<int, int> n2) {
        adj_list[n1].push_back(n2);
        adj_list[n2].push_back(n1);
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
    // Latin square
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
            if (!D[It].size() == 0) {
                int r = rand() % D[It].size();
                auto Iter = D[It].begin();
                advance(Iter, r);
                int color = *Iter;
                V[color].insert(It);
                D[It].erase(Iter);
                square[It.first][It.second] = color;
                // for (int i=0; i<adj_list[It].size(); i++) {
                //     D[adj_list[It][i]].erase(color);   
                // }
            }
        }
    } 

    void Move(pair<int, int> v, int color1, int color2) {
        V[color1].erase(v);
        V[color2].insert(v);
        square[v.first][v.second] = color2;
    }

    int CL() {
        int res = 0;
        for (auto& color_set: V) {
            auto It = color_set.second.begin();
            while (It != color_set.second.end()) {
                auto next_It = It;
                next_It++;
                while(next_It != color_set.second.end()) {
                    if (find(adj_list[*It], *next_It)) {
                        res++;
                    }
                    next_It++;
                }
                It++;
            }
        }
        return res;
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
    cout << test.CL() << "\n";
    return 0;
}