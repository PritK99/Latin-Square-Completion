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
        vector<pair<int, int>> Cand_set; // Candidate cells to color
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

        // Configure already filled cells
        for (int i = 0; i < x; i++) {
            for (int j = 0; j < x; j++) {
                if (square[i][j] != 0) {
                    for (auto It : adj_list[{i, j}]) {
                        D[It].erase(square[i][j]); // Remove filled color from adjacent cells
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
                    }
                }
            }
        }

        // Randomly assign colors to remaining candidates
        // shuffle(Cand_set.begin(), Cand_set.end(), default_random_engine(time(0)));

        srand(time(0));
        for (auto& It : Cand_set) {
            if (D[It].size() != 0) {
                int r = rand() % D[It].size();
                auto Iter = D[It].begin();
                advance(Iter, r); // Get a random color from the domain
                int color = *Iter;
                V[color].insert(It); // Assign color to the cell
                D[It].erase(Iter); // Remove the color from the domain
                square[It.first][It.second] = color; // Update the square
            } else {
                cout << "Given Partial Latin Square cannot be solved\n";
                exit(1); // Exit if no colors are available
            }
        }
    }

    /**
     * @brief Moves a vertex from one color to another.
     *
     * @param v The vertex to move.
     * @param color1 The color to move from.
     * @param color2 The color to move to.
     */
    void Move(pair<int, int> v, int color1, int color2) {
        V[color1].erase(v); // Remove the vertex from the old color
        V[color2].insert(v); // Add the vertex to the new color
        square[v.first][v.second] = color2; // Update the square
    }

    /**
     * @brief Computes the conflict level (CL) of the current state.
     *
     * @return The number of color conflicts.
     */
    int CL() {
        int res = 0;
        for (auto& color_set : V) {
            auto It = color_set.second.begin();
            while (It != color_set.second.end()) {
                auto next_It = It;
                next_It++;
                while (next_It != color_set.second.end()) {
                    if (find(adj_list[*It], *next_It)) {
                        res++; // Increment conflict count if adjacent
                    }
                    next_It++;
                }
                It++;
            }
        }
        return res; // Return total conflicts
    }

    /**
     * @brief Prints the current state of the Latin square.
     */
    void printSquare() {
        int x = square.size();
        for (int i = 0; i < x; i++) {
            for (int j = 0; j < x; j++) {
                cout << square[i][j] << " "; // Print each element
            }
            cout << "\n"; // New line after each row
        }
    }

    /**
     * @brief Checks if the current state is a goal state.
     *
     * @return true if the conflict level is zero, indicating a valid solution.
     */
    bool GoalTest() {
        return CL() == 0; // Return true if no conflicts
    }

    // Function declarations for generating moves
    void MoveGen(queue<LSC>& Q, map<vector<vector<int>>, int>& visited);
    void MoveGen(stack<LSC>& Q, map<vector<vector<int>>, int>& visited);
    void MoveGen(priority_queue<pair<LSC, int>, vector<pair<LSC, int>>, Compare>& Q, map<vector<vector<int>>, int>& visited);
};

/**
 * @brief Comparison class for priority queue sorting.
 */
class Compare {
public:
    /**
     * @brief Overloads the comparison operator to sort by conflict level.
     *
     * @param a First LSC instance with its conflict level.
     * @param b Second LSC instance with its conflict level.
     * @return true if the conflict level of a is less than that of b.
     */
    bool operator() (const pair<LSC, int>& a, const pair<LSC, int>& b) {
        return (a.second < b.second); // Compare based on conflict level
    }
};

/**
 * @brief Generates moves and adds new states to the queue.
 *
 * @param Q The queue to add generated states.
 * @param visited Map tracking visited states.
 */
void LSC::MoveGen(queue<LSC>& Q, map<vector<vector<int>>, int>& visited) {
    vector<int> colors; // List of available colors
    for (int i = 1; i <= square.size(); i++) {
        colors.push_back(i);
    }
    for (auto& color_set : V) {
        for (auto& cell : color_set.second) {
            for (auto& color_next : colors) {
                if (color_next != color_set.first) { // Avoid using the same color
                    LSC neigh; // Create a neighbor LSC
                    neigh.n = n;
                    neigh.adj_list = adj_list;
                    neigh.square = square;
                    neigh.V = V;
                    neigh.D = D;
                    neigh.Move(cell, color_set.first, color_next); // Perform the move
                    if (visited[neigh.square] != 1) {
                        Q.push(neigh); // Add to the queue if not visited
                    }
                }
            }
        }
    }
}

/**
 * @brief Generates moves and adds new states to the stack.
 *
 * @param Q The stack to add generated states.
 * @param visited Map tracking visited states.
 */
void LSC::MoveGen(stack<LSC>& Q, map<vector<vector<int>>, int>& visited) {
    vector<int> colors; // List of available colors
    for (int i = 1; i <= square.size(); i++) {
        colors.push_back(i);
    }
    for (auto& color_set : V) {
        for (auto& cell : color_set.second) {
            for (auto& color_next : colors) {
                if (color_next != color_set.first) { // Avoid using the same color
                    LSC neigh; // Create a neighbor LSC
                    neigh.n = n;
                    neigh.adj_list = adj_list;
                    neigh.square = square;
                    neigh.V = V;
                    neigh.D = D;
                    neigh.Move(cell, color_set.first, color_next); // Perform the move
                    if (visited[neigh.square] != 1) {
                        Q.push(neigh); // Add to the stack if not visited
                    }
                }
            }
        }
    }
}

/**
 * @brief Generates moves and adds new states to the priority queue.
 *
 * @param Q The priority queue to add generated states.
 * @param visited Map tracking visited states.
 */
void LSC::MoveGen(priority_queue<pair<LSC, int>, vector<pair<LSC, int>>, Compare>& Q, map<vector<vector<int>>, int>& visited) {
    vector<int> colors; // List of available colors
    for (int i = 1; i <= square.size(); i++) {
        colors.push_back(i);
    }
    for (auto& color_set : V) {
        for (auto& cell : color_set.second) {
            for (auto& color_next : colors) {
                if (color_next != color_set.first) { // Avoid using the same color
                    LSC neigh; // Create a neighbor LSC
                    neigh.n = n;
                    neigh.adj_list = adj_list;
                    neigh.square = square;
                    neigh.V = V;
                    neigh.D = D;
                    neigh.Move(cell, color_set.first, color_next); // Perform the move
                    if (visited[neigh.square] != 1) {
                        Q.push({neigh, CL() - neigh.CL()}); // Add to the priority queue with priority based on conflict reduction
                    }
                }
            }
        }
    }
}
