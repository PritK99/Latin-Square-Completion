#include <omp.h>
#include "plits.hpp"

using namespace std;

/**
 * @brief Executes the PLITS algorithm to optimize the given LSC.
 *
 * @param S The initial LSC structure to optimize.
 * @return LSC The best solution found after optimization.
 */
LSC PLITS(LSC S, int& steps)
{
    priority_queue<pair<LSC, int>, vector<pair<LSC, int>>, Compare> Open;
    srand(time(0));
    int T = (rand() % 9) + 0.6 * (S.V[0].size() + S.CL());
    map<vector<vector<int>>, int> tabu_list;
    LSC final = S;
    float phi = 1;

    Open.push({S, S.F(phi)});

    // First phase of the PLITS algorithm
    for (int i = 0; i < (50 * S.V.size()) && !Open.empty(); i++)
    {
        steps++;
        auto curr = Open.top();
        Open.pop();
        curr.first.MoveGen(phi, T, Open, tabu_list);

        // Decrease the tabu tenure for all entries in the tabu list
        for (auto &It : tabu_list)
        {
            It.second--;
        }

        if (final.F(phi) > curr.first.F(phi))
        {
            final = curr.first;
            if (final.GoalTest()) return final;
        }
    }

    phi = 10 * S.V.size();
    Open = priority_queue<pair<LSC, int>, vector<pair<LSC, int>>, Compare>();
    tabu_list.clear();
    Open.push({S, S.F(phi)});

    // Second phase of the PLITS algorithm
    for (int i = 0; i < (300 * S.V.size()) && !Open.empty(); i++)
    {
        steps++;
        auto curr = Open.top();
        Open.pop();
        curr.first.MoveGen(phi, T, Open, tabu_list);

        for (auto &It : tabu_list)
        {
            It.second--;
        }

        if (final.F(phi) > curr.first.F(phi))
        {
            final = curr.first;
            if (final.GoalTest()) return final;
        }
    }

    return final;
}

int main(int argc, char *argv[])
{
    int num_threads = 16; // Default number of threads
    if (argc > 1)
    {
        num_threads = stoi(argv[1]);
    }
    omp_set_num_threads(num_threads);

    cout << "Number of threads: " << omp_get_max_threads() << endl;

    LSC test(
        // {{3, 0, 0, 0, 0},
        //  {0, 0, 0, 2, 0},
        //  {0, 0, 1, 0, 0},
        //  {0, 0, 0, 0, 0},
        //  {0, 0, 0, 1, 0},
        // }
        {{1, 0, 0, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0},
         {3, 0, 0, 0},
        }
        // {{3, 0, 0},
        //  {0, 0, 0},
        //  {1, 0, 0}}
    );

    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    vector<LSC> res = test.gen_population(5);
    // for (LSC &i : res)       // Printing the intermediate states
    // {
    //     i.printSquare();
    //     cout << "\n";
    // }
    set<vector<vector<int>>> imp;
    int global_steps = 0;
    #pragma omp parallel for
    for (LSC &i : res)
    {
        int steps = 0;
        LSC temp = PLITS(i, steps);
        #pragma omp critical
        global_steps += steps;
        if (temp.count_zero() == 0)
            imp.insert(temp.square);
    }

    cout << "Unique solutions: \n";

    for (auto &i : imp)
    {
        printSquare(i);
        cout << "\n";
    }

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    cout << "Steps taken (average): " << global_steps << "\n";
    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
    return 0;
}
