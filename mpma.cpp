#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <random>   
#include <algorithm>
#include <queue>
#include <stack>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <list>

using namespace std;

class Compare;

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

    void remove_vertex(pair<int, int> i) {
        n--;
        adj_list.erase(i);
        for (auto& x: adj_list) {
            for (int j=0; j<x.second.size(); j++) {
                if (x.second[j] == i) x.second.erase(x.second.begin() + j);
            }
        }
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
    // No. of cells impossible to fill (D(u) = {0})
    int l = 0;
    // Empty cells (candidates for color filling)
    vector<pair<int, int>> Cand_set;

    LSC() {
        ;
    }

    void init() {
        // Randomly assign color sets to remaining candidates
        shuffle(Cand_set.begin(), Cand_set.end(), default_random_engine(time(0)));
        // srand(time(0));
        for (auto& It: Cand_set) {
            if (D[It].size() != 0) {
                int r = rand() % D[It].size();
                auto Iter = D[It].begin();
                advance(Iter, r);
                int color = *Iter;
                V[color].insert(It);
                D[It].erase(Iter);
                square[It.first][It.second] = color;
            }
        }
    }

    vector<LSC> gen_population(int pop_size) {
        vector<LSC> res;
        for (int i=0; i<pop_size; i++) {
            LSC temp;
            temp.square = square;
            temp.D = D;
            temp.V = V;
            temp.n = n;
            temp.l = l;
            temp.Cand_set = Cand_set;
            temp.adj_list = adj_list;
            temp.init();
            res.push_back(temp);
        }
        return res;
    }

    void operator=(const LSC& L) {
        square = L.square;
        D = L.D;
        V = L.V;
        n = L.n;
        l = L.l;
        Cand_set = L.Cand_set;
        adj_list = L.adj_list;
    }

    LSC(vector<vector<int>> partial) {
        int x = partial.size();
        n = x * x;
        square = partial;
        for (int i=0; i<x; i++) {
            for (int j=0; j<x; j++) {
                for (int k=j+1; k<x; k++) {
                    add_edge({i, j}, {i, k});
                    add_edge({j, i}, {k, i});
                }
                if (square[i][j] == 0) {
                    Cand_set.push_back({i, j});
                    for (int k=0; k<=x; k++) {
                        D[{i, j}].insert(k);
                    }
                }
            }
        }
        
        // Conf for already filled cells
        for (int i=0; i<x; i++) {
            for (int j=0; j<x; j++) {
                if (square[i][j] != 0) {
                    for (auto It : adj_list[{i, j}]) {
                        D[It].erase(square[i][j]);
                    }
                    V[square[i][j]].insert({i, j});
                    // remove_vertex({i, j});
                }
            }
        }

        // Reduction of domain set using smart solving
        bool isReducible = true;
        while (isReducible) {
            isReducible = false;
            for (int i=0; i<x; i++) {
                for (int j=0; j<x; j++) {
                    // Only one possible color can be assigned
                    if (D[{i, j}].size() == 1)
                    {
                        isReducible = true;
                        int color = *D[{i, j}].begin();
                        V[color].insert({i, j});
                        // D[{i, j}].erase(color);
                        square[i][j] = color;
                        if (color == 0) l++;
                        else {
                            for (auto It : adj_list[{i, j}]) {
                                D[It].erase(color);
                            }
                        }
                        pair<int, int> n1 = {i, j};
                        // Removing the cell from the candidate set
                        Cand_set.erase(remove(Cand_set.begin(), Cand_set.end(), n1), Cand_set.end());
                        remove_vertex({i, j});
                    }
                }
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

    int f() {
        return V[0].size();
    }

    int F(float phi) {
        return f() + phi * CL();
    }

    void print_domains() {
        int x = square.size();
        for (auto& It: adj_list) {
            cout << "(" << It.first.first << "," << It.first.second << "): ";
            for (auto It2: D[It.first]) {
                cout << It2 << " ";
            }
            cout << "\n";
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

    bool GoalTest() {
        if (CL() == 0) {
            return true;
        }
        return false;
    }

    void MoveGen(priority_queue<pair<LSC, int>, vector<pair<LSC, int>>, Compare>& Q, map<vector<vector<int>>, int>& visited);
    void MoveGen(int phi, int tt, priority_queue<pair<LSC, int>, vector<pair<LSC, int>>, Compare>& Q, map<vector<vector<int>>, int>& tabu);

};

class Compare {
    public:
        bool operator() (const pair<LSC, int>& a, const pair<LSC, int>& b) {
            return (a.second > b.second);
        }
};

void LSC::MoveGen(priority_queue<pair<LSC, int>, vector<pair<LSC, int>>, Compare>& Q, map<vector<vector<int>>, int>& visited) {
    vector<int> colors;
    for (int i=1; i<=square.size(); i++) {
        colors.push_back(i);
    }
    for (auto& color_set: V) {
        for (auto& cell: color_set.second) {
            for (auto& color_next: colors) {
                if (color_next != color_set.first) {
                    LSC neigh;
                    neigh.n = n;
                    neigh.adj_list = adj_list;
                    neigh.square = square;
                    neigh.V = V;
                    neigh.D = D;
                    neigh.l = l;
                    neigh.Move(cell, color_set.first, color_next);
                    if (visited[neigh.square] != 1) {
                        Q.push({neigh, CL() - neigh.CL()});
                    }
                }
            }
        }
    }
}

void LSC::MoveGen(int phi, int tt, priority_queue<pair<LSC, int>, vector<pair<LSC, int>>, Compare>& Q, map<vector<vector<int>>, int>& tabu) {
    vector<int> colors;
    for (int i=1; i<=square.size(); i++) {
        colors.push_back(i);
    }
    for (auto& color_set: V) {
        for (auto& cell: color_set.second) {
            for (auto& color_next: colors) {
                if (color_next != color_set.first) {
                    LSC neigh;
                    neigh.n = n;
                    neigh.adj_list = adj_list;
                    neigh.square = square;
                    neigh.V = V;
                    neigh.D = D;
                    neigh.l = l;
                    neigh.Move(cell, color_set.first, color_next);
                    if (tabu[neigh.square] == 0) {
                        Q.push({neigh, neigh.F(phi)});
                    }
                    tabu[neigh.square] = tt;
                }
            }
        }
    }
}

LSC PLITS(LSC S) {
    priority_queue<pair<LSC, int>, vector<pair<LSC, int>>, Compare> Open;
    int T = (rand() % 9) + 0.6 * (S.V[0].size() + S.CL());
    map<vector<vector<int>>, int> tabu_list;
    // map<vector<vector<int>>, int> Close;
    LSC final = S;
    float phi = 0.5;
    Open.push({S, S.F(phi)});
    cout << final.CL() << "," << final.F(phi) << "\n";
    for (int i=0; i<100 * S.V.size() && !Open.empty(); i++) {
        auto curr = Open.top();
        // curr.first.printSquare(); 
        // cout << curr.first.F(phi) << "\n";
        Open.pop();
        curr.first.MoveGen(phi, T, Open, tabu_list);
        for (auto& It: tabu_list) {
            It.second--;
        }
        if (final.F(phi) > curr.first.F(phi)) {
            final = curr.first;
        }
    }
    phi = S.V.size();
    final.printSquare();
    cout << final.CL() << "\n";
    Open = priority_queue<pair<LSC, int>, vector<pair<LSC, int>>, Compare>();
    tabu_list.clear();
    Open.push({S, S.F(phi)});
    for (int i=0; i<2 * S.V.size() && !Open.empty(); i++) {
        auto curr = Open.top();
        // curr.first.printSquare(); 
        // cout << curr.first.F(phi) << "\n";
        Open.pop();
        curr.first.MoveGen(phi, T, Open, tabu_list);
        for (auto& It: tabu_list) {
            It.second--;
        }
        if (final.F(phi) > curr.first.F(phi)) {
            final = curr.first;
        }
    }
    final.printSquare();
    cout << final.CL() << "\n";
    return final;
}

int main() {
    LSC test(
        {{3, 0, 0, 0},
         {2, 0, 0, 0},
         {0, 0, 1, 0},
         {0, 0, 0, 0}
        }
    );

    test.printSquare();
    vector<LSC> res = test.gen_population(5);
    // cout << res.size();
    for (LSC& i: res) {
        i.printSquare();
        cout << "\n";
        LSC temp = PLITS(i);
        // temp.printSquare();
        cout << "\n";
    }

    // x.printSquare();
    

    // std::chrono::time_point<std::chrono::system_clock> start, end;
    // start = std::chrono::system_clock::now();
    // // BFS(test);
    // // DFS(test);
    // BestFS(test);
    // end = std::chrono::system_clock::now();
    // std::chrono::duration<double> elapsed_seconds = end - start;
    // std::time_t end_time = std::chrono::system_clock::to_time_t(end);
 
    // std::cout << "finished computation at " << std::ctime(&end_time)
    //           << "elapsed time: " << elapsed_seconds.count() << "s\n";
    return 0;
}