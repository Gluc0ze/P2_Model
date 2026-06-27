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

std::vector<int> determine_ES(const MSProjectData &data){
    
    const int n = data.num_activities;
    std::vector<int> ES(n,-1);
    
    // initially, start node is fixed
    ES[0] = 0;
    bool changed = true;
    while (changed)
    {
        changed = false;
        
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                // i precedes j
                if (data.matrix_dir_predecessors[i][j] == 1)
                {
                    int new_es = ES[i] + data.durations[i];
                    if (new_es > ES[j]){ES[j] = new_es; changed = true;}
                }
            }
        }
    }
    
    return ES;
}

std::vector<int> determine_LS(const MSProjectData &data, int T)
{
    const int n = data.num_activities;
    std::vector<int> LS(n, T);

    // terminal node (usually last activity)
    LS[n - 1] = T - data.durations[n - 1];

    bool changed = true;
    while (changed)
    {
        changed = false;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                // i precedes j
                if (data.matrix_dir_predecessors[i][j] == 1)
                {
                    int new_ls = LS[j] - data.durations[i];
                    if (LS[i] == T || new_ls < LS[i]) {LS[i] = new_ls; changed = true;}
                }
            }
        }
    }

    return LS;
}
