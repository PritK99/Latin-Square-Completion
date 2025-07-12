#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <cassert>
#include <atomic>
#include <list>
#include <queue>
#include <omp.h>

using namespace std;

// Custom hash for std::pair to be used as a key in unordered_map.
struct pair_hash {
    size_t operator()(const pair<int, int>& p) const {
        return hash<int>()(p.first) ^ (hash<int>()(p.second) << 1);
    }
};

class LSC {
public:
    int N;
    vector<vector<int>> square;

    // --- IMPROVEMENT 1: More Efficient Domain Representation ---
    // Using vector<bool> is much more memory-efficient and cache-friendly than unordered_set.
    // D[cell][k] is true if k is a possible value for the cell. Note: we use 1-based indexing for values.
    unordered_map<pair<int, int>, vector<bool>, pair_hash> D;

    list<pair<int, int>> Cand_set;
    unordered_map<pair<int, int>, vector<pair<int, int>>, pair_hash> adj_list;

    LSC() : N(0) {}

    LSC(const vector<vector<int>>& partial) {
        N = partial.size();
        square = partial;

        // 1. Build the constraint graph and initialize candidate set and domains.
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                // Build row and column constraints
                for (int k = j + 1; k < N; k++) {
                    add_edge({i, j}, {i, k});
                    add_edge({j, i}, {k, i});
                }
                if (square[i][j] == 0) {
                    Cand_set.push_back({i, j});
                    // Initialize domain: all values from 1 to N are possible.
                    D[{i, j}] = vector<bool>(N + 1, true);
                }
            }
        }

        // --- IMPROVEMENT 2: SYMMETRY BREAKING ---
        // If the first row is completely empty, we can fix it to be 1, 2, ..., N.
        // This prunes a massive N! factor from the search space.
        bool first_row_empty = true;
        for (int j = 0; j < N; ++j) {
            if (partial[0][j] != 0) {
                first_row_empty = false;
                break;
            }
        }
        if (first_row_empty) {
            cout << "Applying symmetry breaking on the first row." << endl;
            for (int j = 0; j < N; ++j) {
                int value_to_assign = j + 1;
                square[0][j] = value_to_assign;
                // Remove the assigned cell from the candidate list
                Cand_set.remove({0, j});
                // D is no longer needed for this cell
                D.erase({0, j});
            }
        }

        // 2. Propagate constraints from all filled cells (initial and symmetry-broken).
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (square[i][j] != 0) {
                    propagate_constraint({i, j}, square[i][j]);
                }
            }
        }

        // 3. Run AC-3 for powerful initial pruning.
        if (!ac3()) {
            Cand_set.clear();
        }
    }

    // Helper to propagate a constraint: removes 'value' from neighbors of 'cell'.
    void propagate_constraint(const pair<int, int>& cell, int value) {
        if (adj_list.count(cell)) {
            for (const auto& neighbor : adj_list.at(cell)) {
                if (square[neighbor.first][neighbor.second] == 0) {
                    D[neighbor][value] = false;
                }
            }
        }
    }

    void add_edge(pair<int, int> n1, pair<int, int> n2) {
        adj_list[n1].push_back(n2);
        adj_list[n2].push_back(n1);
    }

    bool revise(const pair<int, int>& ci, const pair<int, int>& cj) {
        bool revised = false;
        // This is a simplified AC-3 revise, which is effective for this problem.
        // It checks if cj is reduced to a single value.
        int cj_domain_size = 0;
        int cj_last_value = -1;
        for(int k=1; k<=N; ++k) {
            if(D.at(cj)[k]) {
                cj_domain_size++;
                cj_last_value = k;
            }
        }

        if (cj_domain_size == 1) {
            if (D.at(ci)[cj_last_value]) {
                D.at(ci)[cj_last_value] = false;
                revised = true;
            }
        }
        return revised;
    }

    bool ac3() {
        queue<pair<pair<int, int>, pair<int, int>>> q;
        for (const auto& cell : Cand_set) {
            if (adj_list.count(cell)) {
                for (const auto& neighbor : adj_list.at(cell)) {
                    if (square[neighbor.first][neighbor.second] == 0) {
                        q.push({cell, neighbor});
                    }
                }
            }
        }

        while (!q.empty()) {
            auto [ci, cj] = q.front();
            q.pop();

            if (revise(ci, cj)) {
                bool domain_empty = true;
                for(int k=1; k<=N; ++k) if(D.at(ci)[k]) { domain_empty = false; break; }

                if (domain_empty) return false;

                if (adj_list.count(ci)) {
                    for (const auto& neighbor : adj_list.at(ci)) {
                        if (square[neighbor.first][neighbor.second] == 0) {
                            q.push({neighbor, ci});
                        }
                    }
                }
            }
        }
        return true;
    }

    // Helper to count the size of a domain represented by vector<bool>
    int get_domain_size(const pair<int, int>& cell) const {
        if (!D.count(cell)) return 0;
        int count = 0;
        const auto& domain = D.at(cell);
        for(int k=1; k<=N; ++k) {
            if(domain[k]) count++;
        }
        return count;
    }

    // Helper for the degree heuristic tie-breaker
    int get_degree(const pair<int, int>& cell) const {
        int degree = 0;
        if (adj_list.count(cell)) {
            for (const auto& neighbor : adj_list.at(cell)) {
                if (square[neighbor.first][neighbor.second] == 0) {
                    degree++;
                }
            }
        }
        return degree;
    }

    bool solve_recursive() {
        if (Cand_set.empty()) return true;

        // --- IMPROVEMENT 3: MRV with Degree Heuristic Tie-breaker ---
        auto it = min_element(Cand_set.begin(), Cand_set.end(), [&](const auto& a, const auto& b) {
            int size_a = get_domain_size(a);
            int size_b = get_domain_size(b);
            if (size_a != size_b) {
                return size_a < size_b;
            }
            // Tie-breaker: prefer the cell with more constraints on other empty cells.
            return get_degree(a) > get_degree(b);
        });

        pair<int, int> cell = *it;
        Cand_set.erase(it);

        vector<int> domain_values;
        for(int k=1; k<=N; ++k) if(D.at(cell)[k]) domain_values.push_back(k);

        for (int color : domain_values) {
            square[cell.first][cell.second] = color;

            // We need a copy of the domains to restore on backtrack
            auto D_backup = D;
            vector<pair<pair<int, int>, int>> changes;
            bool possible = true;

            // Forward Checking
            if (adj_list.count(cell)) {
                for (auto& neighbor : adj_list.at(cell)) {
                    if (square[neighbor.first][neighbor.second] == 0) {
                        if (D[neighbor][color]) {
                            changes.push_back({neighbor, color});
                            D[neighbor][color] = false;
                            if (get_domain_size(neighbor) == 0) {
                                possible = false;
                                break;
                            }
                        }
                    }
                }
            }

            if (possible && solve_recursive()) return true;

            // Backtrack: restore the domains from backup
            for (const auto& change : changes) {
                // change.first is the cell (neighbor), change.second is the color
                D[change.first][change.second] = true;
            }
            square[cell.first][cell.second] = 0; // Un-assign the cell
        }

        Cand_set.push_back(cell);
        return false;
    }

    // The parallel solver now benefits from all the setup improvements.
    bool solve_parallel() {
        if (Cand_set.empty()) return true;

        auto it = min_element(Cand_set.begin(), Cand_set.end(), [&](const auto& a, const auto& b) {
            int size_a = get_domain_size(a);
            int size_b = get_domain_size(b);
            if (size_a != size_b) return size_a < size_b;
            return get_degree(a) > get_degree(b);
        });
        pair<int, int> cell = *it;

        vector<int> domain_values;
        for(int k=1; k<=N; ++k) if(D.at(cell)[k]) domain_values.push_back(k);

        atomic<bool> found_solution(false);

        #pragma omp parallel for schedule(dynamic)
        for (int i = 0; i < domain_values.size(); ++i) {
            if (found_solution.load()) continue;

            int color = domain_values[i];

            LSC local_solver = *this;
            local_solver.square[cell.first][cell.second] = color;

            auto it2 = find(local_solver.Cand_set.begin(), local_solver.Cand_set.end(), cell);
            if (it2 != local_solver.Cand_set.end()) {
                local_solver.Cand_set.erase(it2);
            }

            local_solver.propagate_constraint(cell, color);

            if (local_solver.solve_recursive()) {
                #pragma omp critical
                {
                    if (!found_solution.load()) {
                        *this = local_solver;
                        found_solution.store(true);
                    }
                }
            }
        }
        return found_solution.load();
    }

    void printSquare() const {
        cout << "\n";
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                cout << square[i][j] << " ";
            }
            cout << "\n";
        }
        cout << "\n";
    }
};

vector<vector<int>> read_LSC(const string& filename) {
    ifstream infile(filename);
    if (!infile) {
        cerr << "Error: Could not open file " << filename << "\n";
        exit(1);
    }

    int n;
    infile >> n;
    vector<vector<int>> square(n, vector<int>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            infile >> square[i][j];
    return square;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    string filename = argv[1];
    vector<vector<int>> square = read_LSC(filename);

    cout << "Initializing solver for: " << filename << endl;
    auto start_init = chrono::system_clock::now();
    LSC solver(square);
    auto end_init = chrono::system_clock::now();
    chrono::duration<double> init_seconds = end_init - start_init;
    cout << "Initial square (after AC-3 pre-processing took " << init_seconds.count() << "s):";
    solver.printSquare();

    if(solver.Cand_set.empty() && solver.N > 0) {
        cout << "Puzzle is already solved or AC-3 found it unsolvable.\n";
        return 0;
    }


    auto start_solve = chrono::system_clock::now();

    if (solver.solve_parallel()) {
        cout << "Solution Found:";
        solver.printSquare();

        auto end_solve = chrono::system_clock::now();
        chrono::duration<double> solve_seconds = end_solve - start_solve;

        cout << "Initialization Time: " << init_seconds.count() << "s\n";
        cout << "Solving Time: " << solve_seconds.count() << "s\n";
        cout << "Total Elapsed Time: " << init_seconds.count() + solve_seconds.count() << "s\n";

        ofstream csvOut("results.csv", ios::app);
        if (csvOut.is_open()) {
            csvOut << filename << "," << init_seconds.count() + solve_seconds.count() << "\n";
            csvOut.close();
        }
    } else {
        cout << "No solution found.\n";
    }

    return 0;
}
