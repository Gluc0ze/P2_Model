//
//  data.hpp
//  Model_P2
//
//  Created by guillaume vermeire on 19/06/2026.
//

#ifndef data_hpp
#define data_hpp

#include <stdio.h>
#include <vector>
#include <string>

// Use simple C++ structs to store the problem data from MSLIB
struct MSProjectData
{
    int mt; // max training
    int tb; // training budget
    int td;  // training duration
    double tc;   // training cost
    std::vector<double> training_costs;
    std::vector<int> training_duration;
    
    int num_activities;
    int num_resources;
    int num_skills;
    int num_levels;

    int due_date1;
    int due_date2;

    std::vector<int> durations;
    std::vector<int> num_dir_successors;
    std::vector<int> num_dir_predecessors;
    std::vector<int> num_tot_successors;
    std::vector<int> num_tot_predecessors;
    
    std::vector<std::vector<int>> index_dir_successors;
    std::vector<std::vector<int>> index_dir_predecessors;
    std::vector<std::vector<int>> matrix_dir_successors;
    std::vector<std::vector<int>> matrix_dir_predecessors;
    
    std::vector<std::vector<int>> index_tot_successors;
    std::vector<std::vector<int>> index_tot_predecessors;
    std::vector<std::vector<int>> matrix_tot_successors;
    std::vector<std::vector<int>> matrix_tot_predecessors;
    
    std::vector<std::vector<int>> workforce;
    std::vector<std::vector<int>> workforce_skill_levels;
    std::vector<std::vector<std::vector<int>>> bin_levels;
    std::vector<std::vector<int>> skill_requirements;
    std::vector<int> tot_requirements;
    std::vector<std::vector<int>> skill_level_requirements;
    std::vector<std::vector<std::vector<int>>> level_matrix;
    
    // costs
    double rc_f = 0;
    double rc_v = 0;
    std::vector<std::vector<double>> slc_f;
    std::vector<std::vector<double>> slc_v;
    
    std::vector<int> commonResourceUsage;
    
    // addition
    double hire_cost = 0;
};

struct Training
{
    int starttime = -1;
    int finishtime = -1;
    int resource = -1;
    int skill = -1;
};

struct Solution
{
    std::vector<int> starttimes;
    std::vector<int> finishtimes;
    std::vector<std::vector<std::vector<std::vector<int>>>> assignmnets;
    std::vector<std::vector<int>> rpa;  // resource per activity
    
    // solution characteristics
    std::vector<std::vector<int>> wf;
    std::vector<int> activity_list;
    std::string resource_list;
    std::string skill_list;
    
    int makespan = 9999;
    std::vector<Training> training;
    
    double UB = 0;
    double LB = 0;
    double CPU = 0;
    double GAP = 0;
    
    // determine resource per activity assigned
    void determine_rpa(const int n, const int s, const int r, const int l)
    {
        rpa.resize(n);
        for(int i = 0; i<n; i++)
        {
            std::vector<int> res(r,0);
            std::vector<int> local_rpa;
            for(int k = 0; k<r; k++)
            {
                bool found = false;
                for(int j = 0; j<s; j++)
                {
                    for(int u = 0; u<l; u++)
                    {
                        if(assignmnets[i][j][k][u] == 1){found = true; local_rpa.push_back(k);break;}
                    }
                    if(found == true){break;}
                }
                if(found == true){res[k]=1;}
            }
            rpa[i] = local_rpa;
        }
    }
    
    
};

#endif /* data_hpp */
