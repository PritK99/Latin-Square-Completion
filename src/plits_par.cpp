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
    float phi = 0.6;

    Open.push({S, S.F(phi)});

    // First phase of the PLITS algorithm
    for (int i = 0; i < (100 * S.V.size()) && !Open.empty(); i++)
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

    phi = 2 * S.V.size();
    Open = priority_queue<pair<LSC, int>, vector<pair<LSC, int>>, Compare>();
    tabu_list.clear();
    Open.push({S, S.F(phi)});

    // Second phase of the PLITS algorithm
    for (int i = 0; i < (200 * S.V.size()) && !Open.empty(); i++)
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

/**
 * @brief Calculates the distance between two LSC objects.
 *
 * @param A The first LSC object.
 * @param B The second LSC object.
 * @return int The distance between A and B, or -1 if sizes differ.
 */
int dist(LSC A, LSC B)
{
    int res = 0;
    if (A.square.size() != B.square.size())
        return -1;
    int x = A.square.size();

    for (int i = 0; i < x; i++)
    {
        for (int j = 0; j < x; j++)
        {
            if (A.get_color({i, j}) != B.get_color({i, j}))
                res++;
        }
    }
    return res;
}

/**
 * @brief Computes the distance matrix between two populations of LSC objects.
 *
 * @param init_pop The initial population of LSC objects.
 * @param improved The improved population of LSC objects.
 * @return vector<vector<int>> A matrix containing distances between populations.
 */
vector<vector<int>> dist_matrix_calc(vector<LSC> &init_pop, vector<LSC> &improved)
{
    int n = init_pop.size();
    vector<vector<int>> res(n, vector<int>(n, 0));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            res[i][j] = dist(init_pop[i], improved[j]);
        }
    }
    return res;
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

    LSC test({{3, 0, 0, 0, 0},
         {0, 0, 0, 2, 0},
         {0, 0, 1, 0, 0},
         {0, 0, 0, 0, 0},
         {0, 0, 0, 1, 0},
        });

    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    vector<LSC> res = test.gen_population(5);
    // for (LSC &i : res)       // Printing the intermediate states
    // {
    //     i.printSquare();
    //     cout << "\n";
    // }
    vector<LSC> imp;
    int global_steps = 0;
    #pragma omp parallel for
    for (LSC &i : res)
    {
        int steps = 0; 
        LSC temp = PLITS(i, steps);
        #pragma omp critical
        global_steps += steps;
        imp.push_back(temp);
    }
    vector<vector<int>> dist_mat = dist_matrix_calc(res, imp), dist_mat_int = dist_matrix_calc(res, res);

    // P_all
    vector<pair<LSC, pair<int, int>>> P_all;
    for (int i = 0; i < res.size(); i++)
    {
        P_all.push_back({res[i], {0, i}});
    }

    for (int i = 0; i < imp.size(); i++)
    {
        P_all.push_back({imp[i], {1, i}});
    }

    vector<pair<LSC, pair<int, int>>> new_pop;

    sort(P_all.begin(), P_all.end(), [&](pair<LSC, pair<int, int>> &a, pair<LSC, pair<int, int>> &b) { return a.first.CL() < b.first.CL(); });

    // New pop
    new_pop.push_back(P_all[0]);
    P_all.erase(P_all.begin());

    while (new_pop.size() < 5)
    {
        auto x = P_all[0];
        int dist = INT32_MAX;
        for (auto i : new_pop)
        {
            if (x.second.first == 0 && i.second.first == 0)
                dist = min(dist, dist_mat_int[i.second.second][x.second.second]);
            else
            {
                dist = min(dist, dist_mat[i.second.second][x.second.second]);
            }
        }
        if (dist > 5 / 2)
        {
            new_pop.push_back(x);
            P_all.erase(P_all.begin());
        }
    }

    // cout << "Solutions: \n";

    // for (auto &i : new_pop)
    // {
    //     i.first.printSquare();
    //     cout << "\n";
    // }

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    cout << "Steps taken (average): " << global_steps << "\n";
    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
    return 0;
}