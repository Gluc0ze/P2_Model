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
    int si = 0;        // start index
    int ei = 10;        // end index (max 240)
    double timelimit = 30;
    double budget_percentage = 0.10;
    
    //std::vector<std::pair<int, int>> file_indexes = input_selection(false);
    
    // error handling
    if (argc < 4)
    {
        std::cerr << "Usage: ./Model_P2 -si <start index> -ei <end index> -t <runtime>\n";
        //return 1;
    }
    else{HPC = true;}
    
    for (int i = 1; i < argc; i++)
    {
        std::string flag = argv[i];
        if (flag == "-si" && i + 1 < argc){si = std::stoi(argv[i + 1]);i++;}
        else if (flag == "-ei" && i + 1 < argc){ei = std::stoi(argv[i + 1]);i++;}
        else if (flag == "-t" && i + 1 < argc){timelimit = std::stod(argv[i + 1]);i++;}
        else if (flag == "-b" && i + 1 < argc){budget_percentage = std::stod(argv[i + 1]);i++;}
    }

    // input selection
    std::vector<std::pair<int, int>> selection = input_selection(HPC);
    
    //for(int f = bf; f<=ef; f++)
    for(int f = si; f<=ei; f++)
    {
        int file = selection[f].second;
        int set = selection[f].first;
        MSProjectData project = parse_msrcp_file(set, file, HPC);
        //MSProjectData project = parse_msrcp_file(3, 296, HPC);
        completeProjectData(project);
        
        // currently on 10% of max budget and mean of duration
        completeTrainingData(project, budget_percentage);
        std::vector<int> trd(project.num_skills,project.td);
        project.training_duration = trd;
        project.training_costs = {1,1,1,1};
    
        //Solution result = Model_CAT_TRAIN_Vermeire_P2(project,timelimit);
        Solution result = ContiniousModel_CAT_TRAIN_Vermeire(project,timelimit);
        result.set = set;
        result.file = file;
        //printf("Final Result;%d;%d;%d\n",set,f, result.makespan);
        printf("Gurobi Result; (%d); %d; %d; %.2f;%.2f; %.2f;%.2f\n",
               f,
               set,
               file,
               result.LB,
               result.UB,
               result.GAP,
               result.CPU);
        
        solution_output_model(result, HPC,budget_percentage);

    }
    
}
