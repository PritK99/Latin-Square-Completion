#include <climits>
#include <cstdio>
#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <random>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <omp.h>
#include <fstream>
#include <filesystem>

using namespace std;

class Compare;

/**
 * @brief Searches for an element in a vector.
 *
 * @tparam T Type of elements in the vector.
 * @param v The vector to search in.
 * @param e The element to find.
 * @return true if the element is found, false otherwise.
 */
template <typename T>
bool find(vector<T>& v, T e) {
    for (auto& It: v) {
        if (It == e) return true; // Check if the element exists
    }
    return false; // Element not found
}

/**
 * @brief Represents a graph using an adjacency list.
 */
class Graph {
public:
    int n; // Number of vertices
    map<pair<int, int>, vector<pair<int, int>>> adj_list; // Adjacency list

    /**
     * @brief Adds an edge between two vertices.
     *
     * @param n1 The first vertex.
     * @param n2 The second vertex.
     */
    void add_edge(pair<int, int> n1, pair<int, int> n2) {
        adj_list[n1].push_back(n2); // Add n2 to n1's adjacency list
        adj_list[n2].push_back(n1); // Add n1 to n2's adjacency list
    }

    void remove_edge(pair<int, int> n1, pair<int, int> n2) {
        adj_list[n1].erase(remove(adj_list[n1].begin(), adj_list[n1].end(), n2), adj_list[n1].end()); // Remove n2 from n1's adjacency list
        adj_list[n2].erase(remove(adj_list[n2].begin(), adj_list[n2].end(), n1), adj_list[n2].end()); // Remove n1 from n2's adjacency list
    }

    /**
     * @brief Prints the graph's adjacency list.
     */
    void print_graph() {
        for (auto& x: adj_list) {
            cout << "(" << x.first.first << ", " << x.first.second << ")" << ": ";
            for (auto y: x.second) {
                cout << "(" << y.first << ", " << y.second << ")" << " ";
            }
            cout << "\r\n"; // New line after each vertex
        }
    }
};

/**
 * @brief Represents a Latin square coloring problem using graph coloring.
 */
class LSC : public Graph {
public:
    vector<vector<int>> square; // Latin square representation
    map<pair<int, int>, set<int>> D; // Domain set for each cell
    map<int, set<pair<int, int>>> V; // Color classes
    vector<pair<int, int>> Cand_set; // Candidate cells to color

    /**
     * @brief Default constructor.
     */
    LSC() {
        ;
    }

    /**
     * @brief Constructor for initializing LSC with a partial Latin square.
     *
     * @param partial The initial partial Latin square.
     */
    LSC(vector<vector<int>> partial) {
        int x = partial.size(); // Size of the square
        n = x * x; // Total number of cells
        square = partial; // Set the square

        // Build adjacency list and initialize candidate set
        for (int i = 0; i < x; i++) {
            for (int j = 0; j < x; j++) {
                for (int k = j + 1; k < x; k++) {
                    add_edge({i, j}, {i, k}); // Add horizontal edges
                    add_edge({j, i}, {k, i}); // Add vertical edges
                }

                if (square[i][j] == 0) {
                    Cand_set.push_back({i, j}); // Add empty cells to candidate set
                    for (int k = 1; k <= x; k++) {
                        D[{i, j}].insert(k); // Initialize domain set for empty cells
                    }
                }
            }
        }
        for (int i = 0; i < x; i++) {
            for (int j = 0; j < x; j++) {
                if (square[i][j] != 0) {
                    vector<pair<int, int>> list = adj_list[{i, j}];
                    for (pair<int, int> It : list) {
                        D[It].erase(square[i][j]); // Remove filled color from adjacent cells
                        remove_edge(It, make_pair(i, j));
                    }
                    V[square[i][j]].insert({i, j}); // Assign the filled color
                }
            }
        }

        // Reduce domain set using smart solving
        bool isReducible = true;
        while (isReducible) {
            isReducible = false;
            for (int i = 0; i < x; i++) {
                for (int j = 0; j < x; j++) {
                    // Only one possible color can be assigned
                    if (D[{i, j}].size() == 1) {
                        int color = *D[{i, j}].begin(); // Get the only color
                        V[color].insert({i, j}); // Assign color to the cell
                        D[{i, j}].erase(color); // Remove color from the domain
                        square[i][j] = color; // Update the square
                        for (auto It : adj_list[{i, j}]) {
                            D[It].erase(color); // Update adjacent cells' domains
                        }
                        isReducible = true; // Continue reducing
                        pair<int, int> n1 = {i, j};
                        // Remove the cell from the candidate set
                        Cand_set.erase(remove(Cand_set.begin(), Cand_set.end(), n1), Cand_set.end());

                        // Update graph
                        vector<pair<int, int>> list = adj_list[n1];
                        for (pair<int, int> It : list) {
                            D[It].erase(square[i][j]); // Remove filled color from adjacent cells
                            remove_edge(It, n1);
                        }
                    }
                }
            }
        }
    }

    void gen_solns() {
        vector<pair<int, int>> temp_set(Cand_set.begin(), Cand_set.end());
        std::random_device r;
        std::mt19937 eng(r());
        shuffle(temp_set.begin(), temp_set.end(), eng);

        vector<set<pair<int, int>>> MISSets;
        set<pair<int, int>> S;

        // Generate MIS sets with size limit n
        while (!temp_set.empty()) {
            for (auto& cell : temp_set) {
                if (S.size() == square.size()) break; // Restrict MIS size to n
                bool isIndependent = true;

                // Check if cell is independent
                for (auto& selected : S) {
                    if (find(adj_list[selected], cell)) {
                        isIndependent = false;
                        break;
                    }
                }
                if (isIndependent) {
                    S.insert(cell);
                }
            }

            // Remove selected cells from temp_set
            for (auto& cell : S) {
                temp_set.erase(remove(temp_set.begin(), temp_set.end(), cell), temp_set.end());
            }
            MISSets.push_back(S);
            S.clear();
        }

        // Assign colors to each MIS
        for (auto& mis : MISSets) {
            for (auto& cell : mis) {
                for (int color : D[cell]) {
                    bool isValid = true;

                    // Check constraints (row and column)
                    for (auto& neighbor : adj_list[cell]) {
                        if (square[neighbor.first][neighbor.second] == color) {
                            isValid = false;
                            break;
                        }
                    }

                    if (isValid) {
                        // Assign color and update neighbors
                        square[cell.first][cell.second] = color;
                        V[color].insert(cell);
                        for (auto& neighbor : adj_list[cell]) {
                            D[neighbor].erase(color);
                        }
                        break;
                    }
                }

                // Remove the cell from Cand_set
                temp_set.erase(remove(temp_set.begin(), temp_set.end(), cell), temp_set.end());
            }
        }
    }

    int count_zero() {
        int res = 0;
        for (int i=0; i<square.size(); i++) {
            for (int j=0; j<square.size(); j++) {
                if (square[i][j] == 0) res++;
            }
        }

        return res;
    }

    /**
     * @brief Prints the current state of the Latin square.
     */
    void printSquare() const {
        cout << "\n";
        int x = square.size();
        for (int i = 0; i < x; i++) {
            for (int j = 0; j < x; j++) {
                cout << square[i][j] << " "; // Print each element
            }
            cout << "\n"; // New line after each row
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

    std::string filename_basename = std::filesystem::path(filename).filename().string(); // just "LSC-..." without the path

    LSC x = LSC(square);

    x.printSquare();

    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

    bool completed = false;
    int max_iterations = 1e6; // Limit retries

    # pragma omp parallel for shared(completed, max_iterations)
    for (auto i=0; i<max_iterations; i++) {
        LSC curr = LSC(x.square);

        if (completed) continue;
        curr.gen_solns();

        if (curr.count_zero() == 0) {
            #pragma omp critical
            {
                completed = true;
                curr.printSquare();
                std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
                std::chrono::duration<double> elapsed_seconds = end - start;

                std::ofstream csvOut("results/times.csv", std::ios::app); // append mode
                if (csvOut.is_open()) {
                    csvOut << filename << "," << elapsed_seconds.count() << "\n";
                    csvOut.close();
                }

                std::cout << "Elapsed time: " << elapsed_seconds.count() << "s\n";
            }
        }
    }

    return 0;
}
