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
                if (square[i][j] == 0) {
                    Cand_set.push_back({i, j});
                    for (int k=1; k<=x; k++) {
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
                }
            }
        }

        // Reduction of domain set using smart solving
        bool isReducible = true;
        while (isReducible)
        {
            isReducible = false;
            for (int i=0; i<x; i++) 
            {
                for (int j=0; j<x; j++) 
                {
                    // Only one possible color can be assigned
                    if (D[{i, j}].size() == 1)
                    {
                        int color = *D[{i, j}].begin();
                        V[color].insert({i, j});
                        D[{i, j}].erase(color);
                        square[i][j] = color;
                        for (auto It : adj_list[{i, j}]) {
                            D[It].erase(color);
                        }
                        V[color].insert({i, j});
                        isReducible = true;
                        pair <int, int> n1 = {i, j};
                        // Removing the cell from the candidate set
                        Cand_set.erase(remove(Cand_set.begin(), Cand_set.end(), n1), Cand_set.end());
                    }
                }
            }
        }

        // Randomly assign color sets to remaining candidates
        shuffle(Cand_set.begin(), Cand_set.end(), default_random_engine(time(0)));
        srand(time(0));
        for (auto& It: Cand_set) {
            // cout << It.first << "," << It.second << " ";
            if (D[It].size() != 0) {
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
                // cout << color << "\n";
            }
            else {
                cout << "Given Partial Latin Square cannot solved\n";
                exit(1);
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

    bool GoalTest() {
        if (CL() == 0) {
            return true;
        }
        return false;
    }

    void MoveGen(queue<LSC>& Q, map<vector<vector<int>>, int>& visited);
    void MoveGen(stack<LSC>& Q, map<vector<vector<int>>, int>& visited);
    void MoveGen(priority_queue<pair<LSC, int>, vector<pair<LSC, int>>, Compare>& Q, map<vector<vector<int>>, int>& visited);

};

class Compare {
    public:
        bool operator() (const pair<LSC, int>& a, const pair<LSC, int>& b) {
            return (a.second < b.second);
        }
};

void LSC::MoveGen(queue<LSC>& Q, map<vector<vector<int>>, int>& visited) {
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
                    neigh.Move(cell, color_set.first, color_next);
                    if (visited[neigh.square] != 1) {
                        Q.push(neigh);
                    }
                }
            }
        }
    }
}

void LSC::MoveGen(stack<LSC>& Q, map<vector<vector<int>>, int>& visited) {
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
                    neigh.Move(cell, color_set.first, color_next);
                    if (visited[neigh.square] != 1) {
                        Q.push(neigh);
                    }
                }
            }
        }
    }
}

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
                    neigh.Move(cell, color_set.first, color_next);
                    if (visited[neigh.square] != 1) {
                        Q.push({neigh, CL() - neigh.CL()});
                    }
                }
            }
        }
    }
}

void BFS(LSC S) {
    int steps = 0;
    queue<LSC> Open;
    Open.push(S);
    bool solved = false;

    map<vector<vector<int>>, int> Close;
    LSC curr;

    while(!Open.empty()) {
        steps++;
        curr = Open.front();
        curr.printSquare();
        cout << "\n";
        Open.pop();
        if (curr.GoalTest()) {
            solved = true;
            break;
        }
        Close[curr.square]++;

        curr.MoveGen(Open, Close);
    }

    cout << "Soln\n";
    if (solved) {
        cout << "Steps taken: " << steps << "\n";
        curr.printSquare();
    }
}

void DFS(LSC S) {
    int steps = 0;
    stack<LSC> Open;
    Open.push(S);
    bool solved = false;

    map<vector<vector<int>>, int> Close;
    LSC curr;

    while(!Open.empty()) {
        steps++;
        curr = Open.top();
        curr.printSquare();
        cout << "\n";
        Open.pop();
        if (curr.GoalTest()) {
            solved = true;
            break;
        }
        Close[curr.square]++;

        curr.MoveGen(Open, Close);
    }

    cout << "Soln\n";
    if (solved) {
        cout << "Steps taken: " << steps << "\n";
        curr.printSquare();
    }
}

void BestFS(LSC S) {
    int steps = 0;
    priority_queue<pair<LSC, int>, vector<pair<LSC, int>>, Compare> Open;
    Open.push({S, S.CL()});
    bool solved = false;

    map<vector<vector<int>>, int> Close;

    while(!Open.empty()) {
        steps++;
        auto curr = Open.top();
        // curr.first.printSquare();
        // cout << "\n";
        Open.pop();
        if (curr.first.GoalTest()) {
            solved = true;
            curr.first.printSquare();
            break;
        }
        Close[curr.first.square]++;

        curr.first.MoveGen(Open, Close);
    }
    if (solved) {
        cout << "Steps taken: " << steps << "\n";
    }
}

int main() {
    LSC test(
        {{3, 0, 0, 0, 0},
         {0, 0, 0, 0, 0},
         {0, 0, 1, 0, 0},
         {0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0},
        }
    );
    test.printSquare();
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    // BFS(test);
    // DFS(test);
    BestFS(test);
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    // std::time_t end_time = std::chrono::system_clock::to_time_t(end);
 
    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
    return 0;
}