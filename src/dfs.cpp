#include "lsc.hpp"

using namespace std;

/**
 * @brief Performs a Depth-First Search (DFS) on the given Latin square coloring problem.
 *
 * @param S The initial LSC instance to start the search from.
 */
void DFS(LSC S)
{
    int steps = 0;
    stack<LSC> Open;
    Open.push(S);
    bool solved = false;

    map<vector<vector<int>>, int> Close;
    LSC curr;

    while (!Open.empty())
    {
        steps++;
        curr = Open.top();
        curr.printSquare();
        cout << "\n";
        Open.pop();
        if (curr.GoalTest())
        {
            solved = true;
            break;
        }
        Close[curr.square]++;

        curr.MoveGen(Open, Close);
    }

    cout << "Soln\n";
    if (solved)
    {
        cout << "Steps taken: " << steps << "\n";
        curr.printSquare();
    }
}

int main()
{
    LSC test(
        // {{3, 0, 0, 0, 0},
        //  {0, 0, 0, 2, 0},
        //  {0, 0, 1, 0, 0},
        //  {0, 0, 0, 0, 0},
        //  {0, 0, 0, 1, 0},
        // }
        {{3, 0, 0},
         {0, 0, 0},
         {1, 0, 0}});
    test.printSquare();
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    DFS(test);
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;

    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
    return 0;
}