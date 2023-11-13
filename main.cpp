#include <iostream>
#include <map>
#include <vector>

using namespace std;

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
    vector<vector<int>> square;

    LSC() {
        ;
    }

    LSC(vector<vector<int>> partial) {
        int x = partial.size();
        n = x * x;
        square = partial;
        for (int i=0; i<x; i++) {
            for (int j=0; j<x; j++) {
                for (int k=j+1; k<x; k++) {
                    add_edge({i, j}, {i, k});
                }
            }
        }

        for (int i=0; i<x; i++) {
            for (int j=0; j<x; j++) {
                for (int k=j+1; k<x; k++) {
                    add_edge({j, i}, {k, i});
                }
            }
        }
    } 
};

int main() {
    LSC test(
        {{-1, -1, -1, -1},
         {-1, -1, -1, -1},
         {-1, -1, -1, -1},
         {-1, -1, -1, -1}
        }
    );
    test.print_graph();
    return 0;
}