//
//  utils.hpp
//  Model_P2
//
//  Created by guillaume vermeire on 19/06/2026.
//

#ifndef utils_hpp
#define utils_hpp

#include <stdio.h>
#include "data.hpp"
#include <vector>

int sum_vector(const std::vector<int> &vec);
std::vector<int> determine_LS(const MSProjectData &data, int T);
std::vector<int> determine_ES(const MSProjectData &data);
#endif /* utils_hpp */
