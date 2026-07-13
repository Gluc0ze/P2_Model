//
//  input_parser.hpp
//  Model_P2
//
//  Created by guillaume vermeire on 19/06/2026.
//

#ifndef input_parser_hpp
#define input_parser_hpp

#include <string>
#include "data.hpp"

MSProjectData parse_msrcp_file(const int set_ind, const int file_ind, bool HPC);
void completeProjectData(MSProjectData& project);
//void completeTrainingData(MSProjectData& data, const double percentage);
void printParsedData(const MSProjectData& data);
std::vector<std::pair<int, int>> input_selection(bool HPC);
std::vector<std::pair<int, int>> input_selection_set(const int set, bool HPC);
void completeTrainingData(MSProjectData &data,
                          double &percentage);
void solution_output_model(const Solution &sol, bool HPC,double &percentage);
#endif /* input_parser_hpp */

