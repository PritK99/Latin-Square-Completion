#include <climits>
#include <cstdio>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <omp.h>
#include <fstream>
#include <cassert>
#include <atomic>

using namespace std;

struct pair_hash {
    size_t operator()(const pair<int, int>& p) const {
        return hash<int>()(p.first) ^ (hash<int>()(p.second) << 1);
    }
};

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
    unordered_map<pair<int, int>, vector<pair<int, int>>, pair_hash> adj_list;

    void add_edge(pair<int, int> n1, pair<int, int> n2) {
        adj_list[n1].push_back(n2);
        adj_list[n2].push_back(n1);
    }

    void remove_edge(pair<int, int> n1, pair<int, int> n2) {
        adj_list[n1].erase(remove(adj_list[n1].begin(), adj_list[n1].end(), n2), adj_list[n1].end());
        adj_list[n2].erase(remove(adj_list[n2].begin(), adj_list[n2].end(), n1), adj_list[n2].end());
    }

    void print_graph() {
        for (auto& x: adj_list) {
            cout << "(" << x.first.first << ", " << x.first.second << ")" << ": ";
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
    unordered_map<pair<int, int>, unordered_set<int>, pair_hash> D;
    unordered_map<int, unordered_set<pair<int, int>, pair_hash>> V;
    vector<pair<int, int>> Cand_set;

    LSC() {}

    LSC(vector<vector<int>> partial) {
        int x = partial.size();
        n = x * x;
        square = partial;

        for (int i = 0; i < x; i++) {
            for (int j = 0; j < x; j++) {
                for (int k = j + 1; k < x; k++) {
                    add_edge({i, j}, {i, k});
                    add_edge({j, i}, {k, i});
                }

                if (square[i][j] == 0) {
                    Cand_set.push_back({i, j});
                    for (int k = 1; k <= x; k++) {
                        D[{i, j}].insert(k);
                    }
                }
            }
        }
        for (int i = 0; i < x; i++) {
            for (int j = 0; j < x; j++) {
                if (square[i][j] != 0) {
                    vector<pair<int, int>> list = adj_list[{i, j}];
                    for (pair<int, int> It : list) {
                        D[It].erase(square[i][j]);
                        remove_edge(It, make_pair(i, j));
                    }
                    V[square[i][j]].insert({i, j});
                }
            }
        }
    }

    bool solve_recursive() {
        if (Cand_set.empty()) return true;

        auto it = min_element(Cand_set.begin(), Cand_set.end(), [&](auto& a, auto& b) {
            return D[a].size() < D[b].size();
        });

        pair<int, int> cell = *it;
        Cand_set.erase(it);

        for (int color : D[cell]) {
            bool valid = true;
            for (auto& neighbor : adj_list[cell]) {
                int ni = neighbor.first, nj = neighbor.second;
                assert(ni >= 0 && ni < square.size());
                assert(nj >= 0 && nj < square.size());
                if (square[ni][nj] == color) {
                    valid = false;
                    break;
                }
            }
            if (!valid) continue;

            square[cell.first][cell.second] = color;

            vector<pair<pair<int, int>, int>> removed_from_D;
            for (auto& neighbor : adj_list[cell]) {
                if (D[neighbor].erase(color)) {
                    removed_from_D.push_back({neighbor, color});
                }
            }

            if (solve_recursive()) return true;


            for (auto& change : removed_from_D) {
                D[change.first].insert(change.second);
            }
            square[cell.first][cell.second] = 0;
        }

        Cand_set.push_back(cell);
        return false;
    }

    bool solve_parallel() {
        if (Cand_set.empty()) return true;

        auto it = min_element(Cand_set.begin(), Cand_set.end(), [&](auto& a, auto& b) {
            return D[a].size() < D[b].size();
        });

        pair<int, int> cell = *it;
        Cand_set.erase(it);

        vector<int> domain(D[cell].begin(), D[cell].end());
        std::atomic<bool> found_solution(false);

        #pragma omp parallel for schedule(dynamic)
        for (int i = 0; i < domain.size(); ++i) {
            if (found_solution.load()) continue;

            int color = domain[i];

            // Copy state
            LSC local = *this;

            // Validate
            bool valid = true;
            for (auto& neighbor : local.adj_list[cell]) {
                int ni = neighbor.first, nj = neighbor.second;
                if (local.square[ni][nj] == color) {
                    valid = false;
                    break;
                }
            }
            if (!valid) continue;

            local.square[cell.first][cell.second] = color;

            for (auto& neighbor : local.adj_list[cell]) {
                local.D[neighbor].erase(color);
            }

            auto it2 = std::find(local.Cand_set.begin(), local.Cand_set.end(), cell);
            if (it2 != local.Cand_set.end()) local.Cand_set.erase(it2);

            if (local.solve_recursive()) {
                #pragma omp critical
                {
                    if (!found_solution.load()) {
                        *this = local;
                        found_solution.store(true);
                    }
                }
            }
        }

        Cand_set.push_back(cell);
        return found_solution.load();
    }


    int count_zero() {
        int res = 0;
        for (int i = 0; i < square.size(); i++) {
            for (int j = 0; j < square.size(); j++) {
                if (square[i][j] == 0) res++;
            }
        }
        return res;
    }

    void printSquare() const {
        cout << "\n";
        int x = square.size();
        for (int i = 0; i < x; i++) {
            for (int j = 0; j < x; j++) {
                cout << square[i][j] << " ";
            }
            cout << "\n";
        }
        cout << "\n";
    }
};

std::vector<std::vector<int>> read_LSC(const std::string& filename) {
    std::ifstream infile(filename);
    if (!infile) {
        std::cerr << "Error: Could not open file " << filename << "\n";
        exit(1);
    }

    int n;
    infile >> n;
    std::vector<std::vector<int>> square(n, std::vector<int>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            infile >> square[i][j];
    return square;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    std::string filename = argv[1];
    vector<vector<int>> square = read_LSC(filename);
    LSC x = LSC(square);
    x.printSquare();

    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

    bool completed = false;
    int max_iterations = 1e6;

    LSC curr = LSC(x.square);
    if (curr.solve_parallel()) {
        curr.printSquare();
        std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        std::ofstream csvOut("results/times.csv", std::ios::app);
        if (csvOut.is_open()) {
            csvOut << filename << "," << elapsed_seconds.count() << "\n";
            csvOut.close();
        }

        std::cout << "Elapsed time: " << elapsed_seconds.count() << "s\n";
    }
    else {
        std::cout << "No solution found\n";
    }

    return 0;
}
