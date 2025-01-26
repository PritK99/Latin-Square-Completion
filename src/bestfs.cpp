#include "lsc.hpp"

using namespace std;

/**
 * @brief Performs a Best-First Search (BestFS) on the given Latin square coloring problem.
 *
 * @param S The initial LSC instance to start the search from.
 */
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
         {0, 0, 0, 2, 0},
         {0, 0, 1, 0, 0},
         {0, 0, 0, 0, 0},
         {0, 0, 0, 1, 0},
        }
        // {{3, 0, 0},
        //  {0, 0, 0},
        //  {1, 0, 0}}
        // {{1, 0, 0, 0},
        //  {0, 0, 0, 4},
        //  {0, 2, 0, 0},
        //  {3, 0, 0, 0},
        // }
    );
    test.printSquare();
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    BestFS(test);
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
    return 0;
}
