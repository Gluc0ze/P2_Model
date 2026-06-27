//
//  main.cpp
//  Model_P2
//
//  Created by guillaume vermeire on 19/06/2026.
//

#include <iostream>
#include "input_parser.hpp"
#include "data.hpp"
#include "model.hpp"

int main(int argc, const char * argv[])
{
    // arguments
    bool HPC = false;
    int bf = 18;        // create toy example to debug
    int ef = 18;
    int set = 4;
    
    //std::vector<std::pair<int, int>> file_indexes = input_selection(false);
    
    // error handling
    if (argc < 6)
    {
        std::cerr << "Usage: ./Heuristic_P2 -s <set> -bf <begin file> -ef <end file> \n";
        //return 1;
    }
    else{HPC = true;}
    
    for (int i = 1; i < argc; i++)
    {
        std::string flag = argv[i];
        if (flag == "-s" && i + 1 < argc){set = std::stoi(argv[i + 1]);i++;}
        else if (flag == "-bf" && i + 1 < argc){bf = std::stoi(argv[i + 1]);i++;}
        else if (flag == "-ef" && i + 1 < argc){ef = std::stoi(argv[i + 1]);i++;}
    }

    // input selection
    std::vector<std::pair<int, int>> selection = input_selection(false);
    
    //for(int f = bf; f<=ef; f++)
    for(int f = 0; f<1/*selection.size()*/; f++)
    {
        int file = selection[f].second;
        int set = selection[f].first;
        MSProjectData project = parse_msrcp_file(set, file, HPC);
        completeProjectData(project);
        
        // currently on 10% of max budget and mean of duration
        completeTrainingData(project);
        std::vector<int> trd(project.num_skills,project.td);
        project.training_duration = trd;
        project.training_costs = {1,1,1,1};
    
        Solution result = Model_CAT_TRAIN_Vermeire_P2(project);
        //printf("Final Result;%d;%d;%d\n",set,f, result.makespan);
        printf("Gurobi Result; (%d); %d; %d; %.2f;%.2f; %.2f;%.2f\n",
               f,
               set,
               file,
               result.LB,
               result.UB,
               result.GAP,
               result.CPU);
    }
    
}
