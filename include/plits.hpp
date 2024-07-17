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
     * @brief Removes a vertex and its associated edges from the graph.
     * 
     * @param i The vertex to remove.
     */
    void remove_vertex(pair<int, int> i) {
        n--;
        adj_list.erase(i); // Remove the vertex
        for (auto& x: adj_list) {
            for (int j = 0; j < x.second.size(); j++) {
                if (x.second[j] == i) x.second.erase(x.second.begin() + j);
            }
        }
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
    int l = 0; // Number of cells impossible to fill (D(u) = {0})
    vector<pair<int, int>> Cand_set;

    /**
     * @brief Default constructor.
     */
    LSC() {
        ;
    }

    /**
     * @brief Retrieves the color of a specified vertex.
     * 
     * @param vertex The vertex to retrieve the color for.
     * @return The color of the vertex, or 0 if not found.
     */
    int get_color(pair<int, int> vertex) {
        for (int i = 0; i <= square.size(); i++) {
            if (V[i].find(vertex) != V[i].end()) {
                return i;
            }
        }
        return 0;
    }

    /**
     * @brief Initializes the color assignment for empty cells.
     */
    void init() {
        shuffle(Cand_set.begin(), Cand_set.end(), default_random_engine(time(0)));
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

    /**
     * @brief Generates a population of LSC instances based on the current state.
     * 
     * @param pop_size The size of the population to generate.
     * @return A vector containing the generated LSC instances.
     */
    vector<LSC> gen_population(int pop_size) {
        vector<LSC> res; // Resulting population
        #pragma omp parallel for shared(res)
        for (int i = 0; i < pop_size; i++) {
            LSC temp;
            temp.square = square; // Copy current state
            temp.D = D;
            temp.V = V;
            temp.n = n;
            temp.l = l;
            temp.Cand_set = Cand_set;
            temp.adj_list = adj_list;
            temp.init(); // Initialize the new instance
            #pragma omp critical
            res.push_back(temp); // Add to the population
        }
        return res;
    }

    /**
     * @brief Overloads the assignment operator for LSC.
     * 
     * @param L The LSC instance to copy from.
     */
    void operator=(const LSC& L) {
        square = L.square;
        D = L.D;
        V = L.V;
        n = L.n;
        l = L.l;
        Cand_set = L.Cand_set;
        adj_list = L.adj_list;
    }

    /**
     * @brief Constructs an LSC from a partial Latin square.
     * 
     * @param partial The initial partial Latin square.
     */
    LSC(vector<vector<int>> partial) {
        int x = partial.size();
        n = x * x;
        square = partial;

        // Build adjacency list and initialize candidate set
        for (int i = 0; i < x; i++) {
            for (int j = 0; j < x; j++) {
                for (int k = j + 1; k < x; k++) {
                    add_edge({i, j}, {i, k});
                    add_edge({j, i}, {k, i});
                }
                if (square[i][j] == 0) {
                    Cand_set.push_back({i, j}); // Add empty cells to candidate set
                    for (int k = 0; k <= x; k++) {
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
                        isReducible = true;
                        int color = *D[{i, j}].begin(); // Get the only color
                        V[color].insert({i, j}); // Assign color to the cell
                        square[i][j] = color; // Update the square
                        if (color == 0) l++; // Count empty cells
                        else {
                            for (auto It : adj_list[{i, j}]) {
                                D[It].erase(color); // Update adjacent cells' domains
                            }
                        }
                        pair<int, int> n1 = {i, j};
                        Cand_set.erase(remove(Cand_set.begin(), Cand_set.end(), n1), Cand_set.end()); // Remove from candidates
                        remove_vertex({i, j}); // Remove the vertex
                    }
                }
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
        V[color1].erase(v);
        V[color2].insert(v);
        square[v.first][v.second] = color2;
    }

    /**
     * @brief Calculates the number of conflicts in the current coloring.
     * 
     * @return The total number of conflicts.
     */
    int CL() {
        int res = 0; 
        for (auto& color_set: V) {
            if (color_set.first == 0) continue; // Skip empty color
            auto It = color_set.second.begin();
            while (It != color_set.second.end()) {
                auto next_It = It;
                next_It++;
                while (next_It != color_set.second.end()) {
                    if (find(adj_list[*It], *next_It)) {
                        res++; // Count conflicts
                    }
                    next_It++;
                }
                It++;
            }
        }
        return res; // Return total conflicts
    }

    /**
     * @brief Calculates the number of unfilled cells (color 0).
     * 
     * @return The count of unfilled cells.
     */
    int f() {
        return V[0].size();
    }

    /**
     * @brief Calculates the fitness function value.
     * 
     * @param phi A weight parameter for conflict penalty.
     * @return The calculated fitness value.
     */
    int F(float phi) {
        return f() + phi * CL();
    }

    /**
     * @brief Prints the domain set for each cell.
     */
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

    /**
     * @brief Prints the color sets assigned to cells.
     */
    void print_color_set() {
        for (int i = 0; i <= square.size(); i++) {
            cout << i << ": ";
            for (const pair<int, int> i: V[i]) {
                cout << i.first << "," << i.second << " ";
            }
            cout << "\n";
        }
    }

    /**
     * @brief Prints the current state of the Latin square.
     */
    void printSquare() {
        int x = square.size();
        for (int i = 0; i < x; i++) {
            for (int j = 0; j < x; j++) {
                cout << square[i][j] << " ";
            }
            cout << "\n";
        }
    }

    /**
     * @brief Checks if the current state is a goal state (no conflicts).
     * 
     * @return true if the goal state is reached, false otherwise.
     */
    bool GoalTest() {
        return CL() == 0; // Goal if no conflicts
    }

    void MoveGen(priority_queue<pair<LSC, int>, vector<pair<LSC, int>>, Compare>& Q, map<vector<vector<int>>, int>& visited);
    void MoveGen(int phi, int tt, priority_queue<pair<LSC, int>, vector<pair<LSC, int>>, Compare>& Q, map<vector<vector<int>>, int>& tabu);
};

/**
 * @brief Comparator for the priority queue to order LSC instances by their cost.
 */
class Compare {
public:
    bool operator() (const pair<LSC, int>& a, const pair<LSC, int>& b) {
        return (a.second > b.second); // Higher cost has lower priority
    }
};

/**
 * @brief Generates neighbor states by moving colors and adding them to the priority queue.
 * 
 * @param Q The priority queue for storing neighbors.
 * @param visited The map of visited states to avoid duplicates.
 */
void LSC::MoveGen(priority_queue<pair<LSC, int>, vector<pair<LSC, int>>, Compare>& Q, map<vector<vector<int>>, int>& visited) {
    vector<int> colors;
    for (int i = 1; i <= square.size(); i++) {
        colors.push_back(i); // List of possible colors
    }
    for (auto& color_set: V) {
        for (auto& cell: color_set.second) {
            for (auto& color_next: colors) {
                if (color_next != color_set.first) { // Avoid moving to the same color
                    LSC neigh;
                    neigh.n = n; // Copy state
                    neigh.adj_list = adj_list;
                    neigh.square = square;
                    neigh.V = V;
                    neigh.D = D;
                    neigh.l = l;
                    neigh.Move(cell, color_set.first, color_next); // Generate neighbor
                    if (visited[neigh.square] != 1) {
                        Q.push({neigh, CL() - neigh.CL()}); // Add to priority queue if not visited
                    }
                }
            }
        }
    }
}

/**
 * @brief Generates neighbor states considering tabu search to avoid revisiting states.
 * 
 * @param phi Penalty weight.
 * @param tt Tabu tenure.
 * @param Q The priority queue for storing neighbors.
 * @param tabu The map of tabu states.
 */
void LSC::MoveGen(int phi, int tt, priority_queue<pair<LSC, int>, vector<pair<LSC, int>>, Compare>& Q, map<vector<vector<int>>, int>& tabu) {
    vector<int> colors;
    for (int i = 1; i <= square.size(); i++) {
        colors.push_back(i); // List of possible colors
    }
    for (auto& color_set: V) {
        for (auto& cell: color_set.second) {
            for (auto& color_next: colors) {
                if (color_next != color_set.first) { // Avoid moving to the same color
                    LSC neigh;
                    neigh.n = n; // Copy state
                    neigh.adj_list = adj_list;
                    neigh.square = square;
                    neigh.V = V;
                    neigh.D = D;
                    neigh.l = l;
                    neigh.Move(cell, color_set.first, color_next); // Generate neighbor
                    if (tabu[neigh.square] == 0) { // Check if not in tabu list
                        Q.push({neigh, neigh.F(phi)}); // Add to priority queue with fitness
                    }
                    tabu[neigh.square] = tt; // Update tabu tenure
                }
            }
        }
    }
}