//
//  input_parser.cpp
//  Model_P2
//
//  Created by guillaume vermeire on 19/06/2026.
//

#include "input_parser.hpp"
#include "data.hpp"
#include "utils.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <algorithm>  // For std::sort
#include <cstdlib>  // For rand(), srand()
#include <stdexcept>  // For std::invalid_argument
#include <cmath>
#include <random>

std::vector<std::pair<int, int>> input_selection(bool HPC)
{
    std::ifstream infile;
    if(HPC == true){
        //infile.open("/user/gent/479/vsc47914/P2/Model_P2/instance_select/P2_Hardset.txt");
        infile.open("/user/gent/479/vsc47914/P2/Model_P2/instance_select/P2_MSLIB.txt");
    }
    else
    {
        //infile.open ("/Users/guillaumevermeire/Library/CloudStorage/OneDrive-UGent/MacOS/Onderzoek/Paper_3/Code/Select_instances_V2/TestSet_V5.txt");
        //infile.open ("/Users/guillaumevermeire/Library/CloudStorage/OneDrive-UGent/MacOS/Onderzoek/Paper_3/Code/Select_instances_V2/MSLIB_P3_V1.txt");
        //infile.open("/Users/guillaumevermeire/Library/CloudStorage/OneDrive-UGent/MacOS/Onderzoek/Paper_4/Code/TestSet_SP.txt");
        //infile.open("/Users/guillaumevermeire/Library/CloudStorage/OneDrive-UGent/MacOS/Onderzoek/Paper_4/Data/P4_test1.txt");
        infile.open("/Users/guillaumevermeire/Library/CloudStorage/OneDrive-UGent/MacOS/Onderzoek/Paper_2/After Reject 2/Data/P2_testset1.txt");
    }
    std::vector<std::pair<int, int>> out;
    
    //std::ifstream infile("/Users/guillaumevermeire/Library/CloudStorage/OneDrive-UGent/MacOS/Onderzoek/Paper_3/Code/Select_instances_V2/TestSet_V5.txt");  // Change to actual filename/path

        if (!infile || !infile.is_open())
        {
            std::cerr << "Failed to open the input selection." << std::endl;
            return out;
        }

        std::string line;

        // Skip the first line (header)
        std::getline(infile, line);

        // Read the number of instances
        int num_instances;

        infile >> num_instances;

        // Read each (set, file) pair
        int set_id, file_id;
        for (int i = 0; i < num_instances; ++i)
        {
            if (infile >> set_id >> file_id){out.emplace_back(set_id, file_id);}
            else
            {
                std::cerr << "Error reading pair number " << i << std::endl;
                break;
            }
        }
    return out;
}

std::vector<std::pair<int, int>> input_selection_set(const int set, bool HPC)
{
    std::ifstream infile;
    if(HPC == true)
    {
        std::string path = "/user/gent/479/vsc47914/Selection_activities/MSLIB_";
        path.append(std::to_string(set));
        path.append(".txt");
        infile.open(path);
    }
    else
    {
        //infile.open ("/Users/guillaumevermeire/Library/CloudStorage/OneDrive-UGent/MacOS/Onderzoek/Paper_3/Code/Select_instances_V2/TestSet_V5.txt");
        infile.open ("/Users/guillaumevermeire/Library/CloudStorage/OneDrive-UGent/MacOS/Onderzoek/Paper_3/Code/Select_instances_V2/MSLIB_P3_V1.txt");
    }
    std::vector<std::pair<int, int>> out;
    
    //std::ifstream infile("/Users/guillaumevermeire/Library/CloudStorage/OneDrive-UGent/MacOS/Onderzoek/Paper_3/Code/Select_instances_V2/TestSet_V5.txt");  // Change to actual filename/path

        if (!infile || !infile.is_open())
        {
            std::cerr << "Failed to open the file." << std::endl;
            return out;
        }

        std::string line;

        // Skip the first line (header)
        std::getline(infile, line);

        // Read the number of instances
        int num_instances;

        infile >> num_instances;

        // Read each (set, file) pair
        int set_id, file_id;
        for (int i = 0; i < num_instances; ++i)
        {
            if (infile >> set_id >> file_id){out.emplace_back(set_id, file_id);}
            else
            {
                std::cerr << "Error reading pair number " << i << std::endl;
                break;
            }
        }
    return out;
}

MSProjectData parse_msrcp_file(const int set_ind, const int file_ind, bool HPC)
{    // write complete path
    //std::string file_path = "/Users/guillaumevermeire/Library/CloudStorage/OneDrive-UGent/MacOS/Onderzoek/Paper_3/Toy_example/MSLIB_Toy.msrcp";
    //std::string file_path = "/Users/guillaumevermeire/Library/CloudStorage/OneDrive-UGent/MacOS/Onderzoek/MSLIB/MSLIB";
    
    std::string file_path;
    if(HPC==true){file_path = "/user/gent/479/vsc47914/MSLIB";}
    else if(set_ind > 0)
    {
        file_path = "/Users/guillaumevermeire/Desktop/MSLIB/MSLIB";
        file_path.append(std::to_string(set_ind));
    }
    else if(set_ind < 0)
    {
        file_path = "/Users/guillaumevermeire/Desktop/MSLIB/TestFiles";
    }
    //std::string file_path = "/Users/guillaumevermeire/Desktop/MSLIB/MSLIB";
    
    file_path.append("/Instances");
    if(set_ind > 0) {file_path.append(std::to_string(set_ind));}
    file_path.append("/");
    std::string filename = "";
    if(set_ind > 0) {filename = "MSLIB_Set" + std::to_string(set_ind) + "_" + std::to_string(file_ind) + ".msrcp";}
    else {filename = "MSLIB_Test" + std::to_string(file_ind) + ".msrcp";}
    file_path.append(filename);
    
    MSProjectData data;
    std::ifstream file(file_path);
    std::string line;
    
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << file_path << std::endl;
        return data;
    }
    
    // Skip the first comment line
    std::getline(file, line);
    
    // Read: num_activities, num_skills, num_resources, num_modes
    file >> data.num_activities >> data.num_resources >> data.num_skills >> data.num_levels;
    
    // Read release time and due date
    file >> data.due_date1;
    file >> data.due_date2;
    
    // Skip the newline character
    std::getline(file, line); std::getline(file, line);
    
    // Read precedence relations
    data.durations.resize(data.num_activities);
    data.num_dir_successors.resize(data.num_activities);
    data.index_dir_successors.resize(data.num_activities);
    for (int i = 0; i < data.num_activities; ++i) {
        std::getline(file, line);
        std::istringstream iss(line);
        int dur, count;
        iss >> dur >> count;
        data.durations[i] = dur;
        data.num_dir_successors[i] = count;
        for (int j = 0; j < count; ++j)
        {
            int succ = -1;
            iss >> succ;
            data.index_dir_successors[i].push_back(succ-1);
        }
    }
    
    // Skip lines until we find "* Workforce Module *"
    while (std::getline(file, line)) {
        if (line.find("* Workforce Module *") != std::string::npos)
            break;
    }
    
    // Read Workforce Module
    data.workforce.resize(data.num_resources);
    for (int i = 0; i < data.num_resources; ++i) {
        std::getline(file, line);
        std::istringstream iss(line);
        data.workforce[i].resize(data.num_skills);
        for (int j = 0; j < data.num_skills; ++j)
            iss >> data.workforce[i][j];
    }
    
    // Skill Levels
    while (std::getline(file, line)) {
        if (line.find("* Workforce Module with Skill Levels *") != std::string::npos)
            break;
    }
    
    data.workforce_skill_levels.resize(data.num_resources);
    for (int i = 0; i < data.num_resources; ++i) {
        std::getline(file, line);
        std::istringstream iss(line);
        data.workforce_skill_levels[i].resize(data.num_skills);
        for (int j = 0; j < data.num_skills; ++j)
            iss >> data.workforce_skill_levels[i][j];
    }
    
    // Skill Requirements
    while (std::getline(file, line)) {
        if (line.find("* Skill Requirements Module *") != std::string::npos)
            break;
    }
    
    data.skill_requirements.resize(data.num_activities);
    for (int i = 0; i < data.num_activities; ++i) {
        std::getline(file, line);
        std::istringstream iss(line);
        data.skill_requirements[i].resize(data.num_skills);
        for (int j = 0; j < data.num_skills; ++j)
            iss >> data.skill_requirements[i][j];
    }
    
    // Skill Level Requirements
    while (std::getline(file, line)) {
        if (line.find("* Skill Level Requirements Module *") != std::string::npos)
            break;
    }
    
    for (int i = 0; i < data.num_activities; ++i) {
        std::getline(file, line);
        std::istringstream iss(line);
        std::vector<int> levels;
        int level;
        while (iss >> level) {
            if (level == -1) break;
            levels.push_back(level);
        }
        data.skill_level_requirements.push_back(levels);
    }
    
    // cost module
    while (std::getline(file, line)) {
        if (line.find("* Cost Module") != std::string::npos)
            break;
    }
    
    std::getline(file, line);
    std::istringstream iss(line);
    iss >> data.rc_f;
    iss >> data.rc_v;
    
    for (int j = 0; j < data.num_skills; ++j)
    {
        std::getline(file, line);
        // skip line if it is empty
        while (line.find_first_not_of(" \t") == std::string::npos){std::getline(file, line);}
        
        std::istringstream iss(line);
        std::vector<double> levels;
        double level;
        while (iss >> level)
        {
            if (level == -1) break;
            levels.push_back(level);
        }
        data.slc_f.push_back(levels);
    }
    
    for (int j = 0; j < data.num_skills; ++j)
    {
        std::getline(file, line);
        // skip line if it is empty
        while (line.find_first_not_of(" \t") == std::string::npos){std::getline(file, line);}
        
        std::istringstream iss(line);
        std::vector<double> levels;
        double level;
        while (iss >> level)
        {
            if (level == -1) break;
            levels.push_back(level);
        }
        data.slc_v.push_back(levels);
    }
    
    // Common Resource Usage module
    while (std::getline(file, line)) {
        if (line.find("* Common Resource Usage Module *") != std::string::npos)
            break;
    }
    
    // Rework module
    while (std::getline(file, line)) {
        if (line.find("* Rework Module *") != std::string::npos)
            break;
    }
    
    file.close();
    return data;
}

void printParsedData(const MSProjectData& data)
{
    std::cout << "\n=== Project Summary ===\n";
    std::cout << "Activities: " << data.num_activities
              << ", Skills: " << data.num_skills
              << ", Resources: " << data.num_resources
              << ", Levels: " << data.num_levels << '\n';

    std::cout << "Due Date 1: " << data.due_date1
              << ", Due Date 2: " << data.due_date2 << "\n\n";

    // =========================
    // Activities + precedence
    // =========================
    std::cout << "=== Activities & Precedence ===\n";
    for (int i = 0; i < data.num_activities; ++i)
    {
        std::cout << "Activity " << i
                  << " | Dur: " << data.durations[i]
                  << " | #Succ: " << data.num_dir_successors[i]
                  << " | Succ: ";

        for (int s : data.index_dir_successors[i])
            std::cout << s << " ";

        std::cout << "| #Pred: " << data.num_dir_predecessors[i]
                  << " | Pred: ";

        for (int p : data.index_dir_predecessors[i])
            std::cout << p << " ";

        std::cout << "\n";
    }

    // =========================
    // Workforce
    // =========================
    std::cout << "\n=== Workforce (skills) ===\n";
    for (int k = 0; k < data.num_resources; k++)
    {
        std::cout << "Res " << k << ": ";
        for (int j = 0; j < data.num_skills; j++)
            std::cout << data.workforce[k][j] << " ";
        std::cout << "\n";
    }

    std::cout << "\n=== Workforce Skill Levels ===\n";
    for (int k = 0; k < data.num_resources; k++)
    {
        std::cout << "Res " << k << ": ";
        for (int j = 0; j < data.num_skills; j++)
            std::cout << data.workforce_skill_levels[k][j] << " ";
        std::cout << "\n";
    }

    // =========================
    // Requirements
    // =========================
    std::cout << "\n=== Skill Requirements per Activity ===\n";
    for (int i = 0; i < data.num_activities; i++)
    {
        std::cout << "Act " << i << ": ";
        for (int j = 0; j < data.num_skills; j++)
            std::cout << data.skill_requirements[i][j] << " ";
        std::cout << "\n";
    }

    std::cout << "\n=== Total Requirements ===\n";
    for (int i = 0; i < data.num_activities; i++)
    {
        std::cout << "Act " << i << ": "
                  << data.tot_requirements[i] << "\n";
    }

    std::cout << "\n=== Skill Level Requirements ===\n";
    for (int i = 0; i < data.num_activities-1; i++)
    {
        std::cout << "Act " << i << ":\n";
        for (int j = 0; j < data.num_skills; j++)
        {
            for (int l = 0; l < data.num_levels; l++)
                std::cout << data.level_matrix[i][j][l] << " ";
            std::cout << "\n";
        }
    }

    // =========================
    // Level matrix (3D)
    // =========================
    std::cout << "\n=== Level Matrix (Activity x Skill x Level) ===\n";
    for (int i = 0; i < data.num_activities; i++)
    {
        std::cout << "Activity " << i << ":\n";
        for (int j = 0; j < data.num_skills; j++)
        {
            std::cout << "  Skill " << j << ": ";
            for (int l = 0; l < data.num_levels; l++)
                std::cout << data.level_matrix[i][j][l] << " ";
            std::cout << "\n";
        }
    }

    // =========================
    // Costs
    // =========================
    std::cout << "\n=== Cost Parameters ===\n";
    std::cout << "Resource cost fixed (rc_f): " << data.rc_f << "\n";
    std::cout << "Resource cost variable (rc_v): " << data.rc_v << "\n";

    std::cout << "\nSkill Level Costs (Fixed):\n";
    for (const auto& row : data.slc_f)
    {
        for (double val : row) std::cout << val << "\t";
        std::cout << "\n";
    }

    std::cout << "\nSkill Level Costs (Variable):\n";
    for (const auto& row : data.slc_v)
    {
        for (double val : row) std::cout << val << "\t";
        std::cout << "\n";
    }

    std::cout << "\n=== End of Parsed Data ===\n";
}

void completeProjectData(MSProjectData& data)
{
    //std::cout << "Complete data called" << std::endl;
    const int n = data.num_activities;
    const int s = data.num_skills;
    const int lv = data.num_levels;
    const int r = data.num_resources;
    
    // determine matrix direct successors
    data.matrix_dir_successors.resize(n,std::vector<int>(n,0));
    for(int i = 0; i<n; i++)
    {
        for(int j = 0; j<data.num_dir_successors[i]; j++)
        {
            data.matrix_dir_successors[i][data.index_dir_successors[i][j]] = 1;
        }
    }
    
    // determine direct predecessors
    data.num_dir_predecessors.resize(n,0);
    data.index_dir_predecessors.resize(n, std::vector<int>());
    data.matrix_dir_predecessors.resize(n,std::vector<int>(n,0));
    for(int i = 0; i<n; i++)
    {
        for(int j = 0; j<data.num_dir_successors[i]; j++)
        {
            int succ = data.index_dir_successors[i][j];
            data.index_dir_predecessors[succ].push_back(i);
            data.num_dir_predecessors[succ]++;
        }
    }
    for(int i = 0; i<n; i++)
    {
        for(int j = 0; j<data.num_dir_predecessors[i]; j++)
        {
            data.matrix_dir_predecessors[i][data.index_dir_predecessors[i][j]] = 1;
        }
    }
    
    // fill total predecessors
    data.matrix_tot_predecessors.resize(n,std::vector<int>(n,0));
    for (int i = 1; i < n; i++)
    {
        for (int h = 0; h < n; h++)
        {
            if(data.matrix_dir_predecessors[i][h] == 1)
            {
                data.matrix_tot_predecessors[i][h] = 1;
                if(h != 0)
                {
                    int h2 = h;
                    for (int g = h2-1; g >= 0; g--)
                    {
                        if(data.matrix_tot_predecessors[h2][g] == 1){data.matrix_tot_predecessors[i][g] = 1;}
                    }
                }
            }
        }
    }
    
    // fill total successors
    data.matrix_tot_successors.resize(n,std::vector<int>(n,0));
    for(int i = 0; i<n; i++)
    {
        for(int p = 0; p<n; p++)
        {
            data.matrix_tot_successors[i][p] = data.matrix_tot_predecessors[p][i];
        }
    }
    
    // fill the rest
    data.index_tot_successors.resize(n, std::vector<int>());
    data.index_tot_predecessors.resize(n, std::vector<int>());
    data.num_tot_successors.resize(n);
    data.num_tot_predecessors.resize(n);
    for(int i = 0; i<n; i++)
    {
        for(int p = 0; p<n;p++)
        {
            if(data.matrix_tot_successors[i][p] == 1){data.index_tot_successors[i].push_back(p);}
            if(data.matrix_tot_predecessors[i][p] == 1){data.index_tot_predecessors[i].push_back(p);}
        }
    }
    for(int i = 0; i<n; i++)
    {
        data.num_tot_successors[i] = (int)data.index_tot_successors[i].size();
        data.num_tot_predecessors[i] = (int)data.index_tot_predecessors[i].size();
    }
    
    // find total reqiurements for activities
    data.tot_requirements.resize(n,0);
    for(int i = 0; i<n; i++)
    {
        for(int j = 0; j<s; j++)
        {
            data.tot_requirements[i] += data.skill_requirements[i][j];
        }
    }
    
    // fix skill level requirements
    //std::vector<std::vector<std::vector<int>>> LM(n,std::vector<std::vector<int>>(s,std::vector<int>(lv+1,0)));   // for skilll level
    std::vector<std::vector<std::vector<int>>> LM(n,std::vector<std::vector<int>>(s,std::vector<int>(lv,0)));   // for index
    for(int i = 0; i<n; i++)
    {
        std::vector<int> required_levels = data.skill_level_requirements[i];
        if(required_levels.size() > 0)
        {
            int count = 0;
            for(int j = 0; j<s; j++)
            {
                int skill = j;
                int req_j = data.skill_requirements[i][j];
                int indx = count;
                for(int x = indx; x< indx+req_j; x++)
                {
                    //int level = required_levels[x];   // represents skill level
                    int level = required_levels[x]-1;   // represents index of skill level
                    LM[i][skill][level]++;
                }
                count += req_j;
            }
        }
    }
    data.level_matrix = LM;
    
    // 3d skill mastery
    std::vector<std::vector<std::vector<int>>> wf_sl(r,std::vector<std::vector<int>>(s,std::vector<int>(lv,0)));
    for(int k = 0; k<r; k++)
    {
        for(int j = 0; j<s; j++)
        {
            int level = data.workforce_skill_levels[k][j]; // do -one for index?
            if(level > 0){ level -= 1; wf_sl[k][j][level] = 1;}
        }
    }
    data.bin_levels = wf_sl;
    
    // give hire cost to data
    data.hire_cost = 200;
}

void completeTrainingData(MSProjectData &data,
                          double &percentage)
{
    const int n = data.num_activities;
    const int r = data.num_resources;
    const int s = data.num_skills;
    const int l = data.num_levels;
    
    // the budget is 10% rounded up of available training
    double max = 0;
    for(int k = 0; k<r;k++){
        for(int j= 0; j<s; j++){
            if(data.workforce[k][j] == 0){
                max++;
            }
        }
    }
    data.mt = max;
    //int budget = static_cast<int>(std::ceil(0.10*max));
    int budget = static_cast<int>(std::ceil(percentage*max));
    data.tb = budget;
    
    // one single training duration is equal to the mean (sorted middle) of all activity durations
    int duration = 0;
    std::vector<int> dur = data.durations;
    std::sort(dur.begin(), dur.end());
    // Odd number of elements
    if (n % 2 == 1){duration = dur[n / 2];};
    // Even number of elements
    duration = (dur[n / 2 - 1] + dur[n / 2]) / 2.0;
    data.td = duration;
    
    // cost is one
    data.tc = 1;
}

void solution_output_model(const Solution &sol, bool HPC,double &percentage)
{
    std::string path = "";
    if(HPC == true)
    {
        //path = "/user/gent/479/vsc47914/P2/Model_P2/results/instances";
        //path.append("/MSLIB");
        path = "/user/gent/479/vsc47914/P2/Model_P2/results/MSLIB_instances_";
        if(percentage == 0){path.append("0");}
        else if(percentage == 0.01){path.append("1");}
        else if(percentage == 0.05){path.append("5");}
        else if(percentage == 0.10){path.append("10");}
        path.append("/MSLIB");
    }
    else
    {
        path = "/Users/guillaumevermeire/Library/CloudStorage/OneDrive-UGent/MacOS/Onderzoek/Paper_2/After Reject 2/Resulties/";
    }
    path.append("Set"+std::to_string(sol.set)+"_"+std::to_string(sol.file)+".txt");
    
    std::ofstream out(path);
    if (!out.is_open())
    {
        std::cerr << "Cannot open file: " << path << std::endl;
        return;
    }

    // check status
    bool status_symbol = false;
    if(sol.status != "NOSOL" && sol.status != "INF"){status_symbol = true;}
    
    // =====================
    // META
    // =====================
    out << "/* Instance information */" << "\n";
    out << "set;" << sol.set << "\n";
    out << "file;" << sol.file << "\n";
    
    // =====================
    // SOLVER INFO
    // =====================
    out << "\n" << "/* Solver information */" << "\n";
    out << "status;" << sol.status << "\n";
    out << "LB;"; if(status_symbol){out << sol.LB;} else{out << "-";}  out << "\n";
    out << "UB;";  if(status_symbol){out << sol.UB;} else{out << "-";}  out << "\n";
    out << "GAP;"; if(status_symbol){out << sol.GAP;} else{out << "-";} out << "\n";
    out << "CPU;"; if(status_symbol){out << sol.CPU;} else{out << "-";} out << "\n";
    out << "TR;"; if(status_symbol){out << sol.percentage_trained;} else{out << "-";} out << "\n";
    
    out.close();
}
