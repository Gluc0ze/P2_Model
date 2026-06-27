//
//  utils.cpp
//  Model_P2
//
//  Created by guillaume vermeire on 19/06/2026.
//

#include "utils.hpp"
#include "data.hpp"
#include <vector>

int sum_vector(const std::vector<int> &vec)
{
    int sum = 0;
    for(int i = 0; i< vec.size(); i++){sum += vec[i];}
    return sum;
}

std::vector<int> determine_ES(const MSProjectData &data)
{
    const int n = data.num_activities;
    std::vector<int> ES(n, 0);

    for(int i = 0; i<n; i++){
        int max_time = 0;
        for(int i2 = 0; i2< data.num_dir_predecessors[i]; i2++){
            int pred = data.index_dir_predecessors[i][i2];
            if(ES[pred]+data.durations[pred]>max_time){max_time = ES[pred]+data.durations[pred];}
        }
        ES[i] = max_time;
    }

    return ES;
}

std::vector<int> determine_LS(const MSProjectData &data, int makespan)
{
    const int n = data.num_activities;
    std::vector<int> LS(n, makespan);

    // reverse topological order assumption
    for (int i = n - 1; i >= 0; i--)
    {
        bool hasSuccessor = false;
        int min_time = makespan;
        for (int k = 0; k < data.num_dir_successors[i]; k++)
        {
            hasSuccessor = true;
            int succ = data.index_dir_successors[i][k];
            int candidate = LS[succ] - data.durations[i];
            if (candidate < min_time){min_time = candidate;}
        }

        // if no successors → end activity
        if (!hasSuccessor)
        {
            LS[i] = makespan - data.durations[i];
        }
        else
        {
            LS[i] = min_time;
        }
    }

    return LS;
}
