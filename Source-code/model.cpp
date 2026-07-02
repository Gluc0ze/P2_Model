//
//  model.cpp
//  Model_P2
//
//  Created by guillaume vermeire on 19/06/2026.
//

#include <stdio.h>
#include "model.hpp"
#include "data.hpp"
#include <vector>
#include "utils.hpp"
#include "gurobi_c++.h"
#include <iomanip>
#include <cmath>

int n = -1;
int r = -1;
int s = -1;
int l = -1;
int T = -1;

Solution Model_CAT_Vermeire_P2(const MSProjectData &data)
{
    Solution sol;
    n = data.num_activities;
    r = data.num_resources;
    s = data.num_skills;
    
    T = sum_vector(data.durations);
    
    std::vector<int> ES = determine_ES(data);   // temp
    std::vector<int> LS = determine_LS(data, data.due_date1);
    
    try
    {
        // create environment
        GRBEnv env = GRBEnv();
        
        // create model
        GRBModel model = GRBModel(env);
        env.set(GRB_IntParam_OutputFlag, 0);
        
        // assignment variables
        GRBVar x[n][s][r][T];
        // starttime
        GRBVar st[n][T];
        
        // create decision variables
        for(int i = 0; i<n; i++){
            for(int j = 0; j<s; j++){
                for(int k = 0; k<r; k++){
                    for(int t = 0; t<=T; t++){
                        x[i][j][k][t] = (model.addVar(0.0, 1.0, 0.0, GRB_BINARY));
                        //x[idx(i,j,k,u,t)] =
                    }
                }
            }
        }
        for(int i = 0; i<n; i++){
            for(int t = 0; t<=T; t++){
                st[i][t] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY);
            }
        }
        
        // constraints
        // resource requirement
        for(int i = 1; i<n-1; i++){
            for(int j = 0; j<s; j++){
                if(data.skill_requirements[i][j]>0){
                    GRBLinExpr rhs = data.skill_requirements[i][j];
                    GRBLinExpr lhs = 0;
                    for(int k = 0; k<r; k++){
                        for(int t = ES[i]; t<= LS[i]; t++){
                            lhs += x[i][j][k][t];
                        }
                    }
                    model.addConstr(lhs, GRB_EQUAL, rhs);
                }
            }
        }
        
        // precedence relations
        for(int i = 0; i<n; i++){
            for(int i2 = 0; i2<n; i2++){
                if(data.matrix_dir_predecessors[i2][i] == 1){
                    for(int t = ES[i]; t <= LS[i]; t++)
                    {
                        GRBLinExpr rhs = 1;
                        GRBLinExpr lhs = 0;
                        for(int t2 = t; t2 <= LS[i]; t2++){lhs += st[i][t2];}
                        for(int t2 = t; t2 <= std::min(LS[i2], t+data.durations[i]-1); t2++){lhs += st[i2][t2];}
                        model.addConstr(lhs, GRB_LESS_EQUAL, rhs);
                    }
                }
            }
        }
        
        // start time dummy node
        GRBLinExpr constr1 = 0;
        constr1 += st[0][0];
        model.addConstr(constr1 == 1);
        
        // should be at least one start time for each activity
        for(int i = 0; i < n; i++)
        {
            GRBLinExpr rhs = 1;
            GRBLinExpr lhs = 0;
            for(int t = ES[i]; t <= LS[i]; t++){lhs += st[i][t];}
            model.addConstr(lhs, GRB_EQUAL, rhs);
        }
        
        // resource can only do one skill at a specific time point and it starts at startpoint
        for(int i = 0; i<n; i++){
            for(int k = 0; k<r; k++){
                for(int t = ES[i]; t <= LS[i]; t++){
                    GRBLinExpr lhs = 0;
                    for(int j = 0; j<s; j++){
                        if(data.skill_requirements[i][j]>0 &&
                           data.workforce[k][j]>0){
                            lhs += x[i][j][k][t];
                        }
                    }
                    GRBLinExpr rhs = st[i][t];
                    model.addConstr(lhs, GRB_LESS_EQUAL, rhs);
                }
            }
        }
        
        // Resource restriction: no parallel assignment
        for (int k = 0; k < r; k++) {
            for (int t = 0; t <= T; t++) {
                GRBLinExpr lhs = 0;
                
                for (int i = 0; i < n; i++) {
                    for (int j = 0; j < s; j++) {
                            if (data.workforce[k][j]>0 &&
                                data.skill_requirements[i][j] > 0)
                            {
                                int tStart = std::max(ES[i], t - data.durations[i] + 1);
                                int tEnd   = std::min(LS[i], t);
                                
                                for (int t2 = tStart; t2 <= tEnd; t2++) {lhs += x[i][j][k][t2];}
                            }
                        }
                    }

                model.addConstr(lhs <= 1, "NoParallel_" + std::to_string(k) + "_" + std::to_string(t));
            }
        }
        
        // optimize
        GRBLinExpr obj = 0;
        for(int t = 0; t<=T; t++)
        {
            obj += st[n-1][t]*t;
        }
        model.setObjective(obj, GRB_MINIMIZE);
        model.optimize();
        
        if (model.get(GRB_IntAttr_SolCount) > 0)
        {
            std::cout << "Objective;"
            << model.get(GRB_DoubleAttr_ObjVal)
            << std::endl;
            
            for(int i = 0; i < n; i++){
                for(int j = 0; j < s; j++){
                    for(int k = 0; k < r; k++){
                        for(int t = 0; t < T; t++){
                            if(x[i][j][k][t].get(GRB_DoubleAttr_X) > 0.5)
                            {
                                std::cout
                                << "Activity " << i
                                << "; Skill " << j
                                << "; Resource " << k
                                << "; Time " << t
                                << std::endl;
                            }
                        }
                    }
                }
            }
        }
        
        for(int i = 0; i < n; i++){
            for(int t = 0; t < T; t++){
                if(st[i][t].get(GRB_DoubleAttr_X) > 0.5){
                    std::cout << "Activity; " << i
                              << " Time; " << t
                              << std::endl;
                }
            }
        }
        
    }
    
    catch (GRBException e)
    {
        std::cout << "Error code = " << e.getErrorCode() << std::endl;
        std::cout << e.getMessage() << std::endl;
    }
    
    
    return sol;
}

Solution Model_CAT_TRAIN_Vermeire_P2(const MSProjectData &data, double timelimit)
{
    Solution sol;
    n = data.num_activities;
    r = data.num_resources;
    s = data.num_skills;
    
    //T = sum_vector(data.durations);
    T = 0;
    for(int i = 0; i<n; i++){
        if(data.durations[i]>T){T = data.durations[i];}
    }
    T += data.due_date1;
    std::vector<int> ES = determine_ES(data);   // temp
    std::vector<int> LS = determine_LS(data, T);
    
    try
    {
        // create environment
        GRBEnv env = GRBEnv();
        
        // create model
        GRBModel model = GRBModel(env);
        env.set(GRB_IntParam_OutputFlag, 0);
        model.set(GRB_IntParam_Threads, 1);
        
        // assignment variables
        //GRBVar x[n][s][r][T+1];
        std::vector<std::vector<std::vector<std::vector<GRBVar>>>> x(n,
            std::vector<std::vector<std::vector<GRBVar>>>(s,
                std::vector<std::vector<GRBVar>>(r,
                    std::vector<GRBVar>(T+1)
                )
            )
        );
        
        // starttime
        GRBVar st[n][T+1];
        
        // training var
        GRBVar b[r][s][T+1];
        GRBVar tr[r][s][T+1];
        
        // create decision variables
        for(int i = 0; i<n; i++){
            for(int j = 0; j<s; j++){
                for(int k = 0; k<r; k++){
                    for(int t = 0; t<=T; t++){
                        x[i][j][k][t] = (model.addVar(0.0, 1.0, 0.0, GRB_BINARY));
                        //x[idx(i,j,k,u,t)] =
                    }
                }
            }
        }
        for(int i = 0; i<n; i++){
            for(int t = 0; t<=T; t++){
                st[i][t] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY);
            }
        }
        
        for(int k = 0; k<r; k++){
            for(int j = 0; j<s; j++){
                for(int t = 0; t<=T;t++){
                    tr[k][j][t] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY);
                    b[k][j][t] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY);
                }
            }
        }
        
        // constraints
        // resource requirement should be met
        for(int i = 1; i<n-1; i++){
            for(int j = 0; j<s; j++){
                if(data.skill_requirements[i][j]>0){
                    GRBLinExpr rhs = data.skill_requirements[i][j];
                    GRBLinExpr lhs = 0;
                    for(int k = 0; k<r; k++){
                        for(int t = ES[i]; t<= LS[i]; t++){
                            lhs += x[i][j][k][t];
                        }
                    }
                    model.addConstr(lhs, GRB_EQUAL, rhs);
                }
            }
        }
        
        // precedence relations
        for(int i = 0; i<n; i++){
            for(int i2 = 0; i2<n; i2++){
                if(data.matrix_dir_predecessors[i2][i] == 1){
                    for(int t = ES[i]; t <= LS[i]; t++)
                    {
                        GRBLinExpr rhs = 1;
                        GRBLinExpr lhs = 0;
                        for(int t2 = t; t2 <= LS[i]; t2++){lhs += st[i][t2];}
                        for(int t2 = t; t2 <= std::min(LS[i2], t+data.durations[i]-1); t2++){lhs += st[i2][t2];}
                        model.addConstr(lhs, GRB_LESS_EQUAL, rhs);
                    }
                }
            }
        }
        
        // start time dummy node
        GRBLinExpr constr1 = 0;
        constr1 += st[0][0];
        model.addConstr(constr1 == 1);
        
        // should be at least one start time for each activity
        for(int i = 0; i < n; i++)
        {
            GRBLinExpr rhs = 1;
            GRBLinExpr lhs = 0;
            for(int t = ES[i]; t <= LS[i]; t++){lhs += st[i][t];}
            model.addConstr(lhs, GRB_EQUAL, rhs);
        }
        
        // resource can only do one skill at a specific time point and it starts at startpoint
        for(int i = 0; i<n; i++){
            for(int k = 0; k<r; k++){
                for(int t = ES[i]; t <= LS[i]; t++){
                    GRBLinExpr lhs = 0;
                    for(int j = 0; j<s; j++){
                        if(data.skill_requirements[i][j]>0 /*&&
                           data.workforce[k][j]>0*/){
                            lhs += x[i][j][k][t];
                        }
                    }
                    GRBLinExpr rhs = st[i][t];
                    model.addConstr(lhs, GRB_LESS_EQUAL, rhs);
                }
            }
        }
        
        // Resource restriction: no parallel assignment
        for (int k = 0; k < r; k++) {
            for (int t = 0; t <= T; t++) {
                GRBLinExpr lhs = 0;
                for (int i = 0; i < n; i++) {
                    for (int j = 0; j < s; j++) {
                        if (/*data.workforce[k][j]>0 &&*/
                            data.skill_requirements[i][j] > 0)
                        {
                            int tStart = std::max(ES[i], t - data.durations[i] + 1);
                            int tEnd   = std::min(LS[i], t);
                            
                            for (int t2 = tStart; t2 <= tEnd; t2++) {lhs += x[i][j][k][t2];}
                        }
                    }
                }
                
                model.addConstr(lhs <= 1, "NoParallel_" + std::to_string(k) + "_" + std::to_string(t));
            }
        }
        
        // Training variables
        // (1.) initial skill mastery
        for(int k=0;k<r;k++){
            for(int j=0;j<s;j++){
                model.addConstr(b[k][j][0] == data.workforce[k][j]);
            }
        }
        
        // (2.) skill can not be lost (compounding)
        for(int k=0;k<r;k++){
            for(int j=0;j<s;j++){
                for(int t=1;t<T;t++){
                    model.addConstr(b[k][j][t] >= b[k][j][t-1]);
                }
            }
        }
        
        // (3.) training effect
        for(int k=0;k<r;k++){
            for(int j=0;j<s;j++){
                for(int t=0;t<T;t++){
                    int lastTau = t - data.training_duration[j];
                    if(lastTau >= 0){
                        for(int tau=0; tau<=lastTau; tau++){
                            model.addConstr(b[k][j][t] >= tr[k][j][tau]);
                        }
                    }
                }
            }
        }
        
        // (4.) prevent spontauous training
        for(int k=0;k<r;k++){
            for(int j=0;j<s;j++){
                for(int t=0;t<T;t++){
                    GRBLinExpr rhs = data.workforce[k][j];
                    int lastTau = t - data.training_duration[j];
                    if(lastTau >= 0){
                        for(int tau=0; tau<=lastTau; tau++){rhs += tr[k][j][tau];}
                    }
                    model.addConstr(b[k][j][t] <= rhs);
                }
            }
        }
        
        // (5.) at most one training per skill
        for(int k=0;k<r;k++){
            for(int j=0;j<s;j++){
                GRBLinExpr lhs = 0;
                for(int t=0;t<T;t++)
                    lhs += tr[k][j][t];
                model.addConstr(lhs <= 1 - data.workforce[k][j]);
            }
        }
        
        // (6.) can only assign when mastering
        for(int i = 0; i<n; i++){
            for(int j = 0; j<s; j++){
                for(int k=0;k<r;k++){
                    for(int t=0;t<T;t++){
                        model.addConstr(x[i][j][k][t] <= b[k][j][t]);
                    }
                }
            }
        }
        
        // (7.) Training budget
        GRBLinExpr used = 0;
        GRBLinExpr budget = data.tb;
        for(int k=0;k<r;k++){
            for(int j=0;j<s;j++){
                for(int t=0;t<=T;t++){
                    used += tr[k][j][t]*data.training_costs[j];
                }
            }
        }
        model.addConstr(used <= budget);
        
        // optimize
        GRBLinExpr obj = 0;
        for(int t = 0; t<=T; t++)
        {
            obj += st[n-1][t]*t;
        }
        model.setObjective(obj, GRB_MINIMIZE);
        model.set(GRB_DoubleParam_TimeLimit, timelimit);
        model.optimize();
        
        int status = model.get(GRB_IntAttr_Status);
        if (status == GRB_OPTIMAL) {sol.status = "OPT";}
        else if (status == GRB_INFEASIBLE){sol.status = "INF";}
        else if (status == GRB_TIME_LIMIT){sol.status = "FEAS";}
        
        if (model.get(GRB_IntAttr_SolCount) > 0)
        {
            // save information
            sol.makespan = model.get(GRB_DoubleAttr_ObjVal);
          
            sol.UB = model.get(GRB_DoubleAttr_ObjVal);      // Best feasible solution
            sol.LB = model.get(GRB_DoubleAttr_ObjBound);        // Best lower bound
            sol.CPU = model.get(GRB_DoubleAttr_Runtime);        // Runtime in seconds
            sol.GAP = model.get(GRB_DoubleAttr_MIPGap);         // Relative MIP gap
            
            //--------------------------------------------------
            // Trainings
            //--------------------------------------------------
            double total_trained = 0;
            for(int k = 0; k < r; k++)
            {
                for(int j = 0; j < s; j++)
                {
                    for(int t = 0; t < T; t++)
                    {
                        if(tr[k][j][t].get(GRB_DoubleAttr_X) > 0.5){total_trained++;}
                    }
                }
            }
            // percentage trained
            sol.percentage_trained = (total_trained/(double)data.tb)*100;
            printf("");
            
        }
        else if (model.get(GRB_IntAttr_SolCount) == 0){
            sol.status = "NOSOL";
            sol.UB = 9999;
            sol.LB = 9999;
            sol.GAP = 9999;
            sol.percentage_trained = 9999;
        }
    }
    
    catch (GRBException e)
    {
        std::cout << "Error code = " << e.getErrorCode() << std::endl;
        std::cout << e.getMessage() << std::endl;
    }
    
    
    return sol;
}

Solution ContiniousModel_CAT_TRAIN_Vermeire(const MSProjectData &data,
                                            double timelimit){
    Solution sol;
    const int n = data.num_activities;
    const int r = data.num_resources;
    const int s = data.num_skills;
    const int M = 99999;
    
    //T = sum_vector(data.durations);
    T = 0;
    for(int i = 0; i<n; i++){
       T += data.durations[i];
    }
    T++;
    
    // possible training pairs
    std::vector<std::pair<int, int>> training_set;
    for(int k = 0; k<r; k++){
        for(int j = 0; j<s; j++){
            if(data.workforce[k][j]>0){training_set.push_back({k,j});}
        }
    }
    int h = (int)training_set.size();
    
    try
    {
        // create environment
        GRBEnv env = GRBEnv();
        
        // create model
        GRBModel model = GRBModel(env);
        //env.set(GRB_IntParam_OutputFlag, 0);
        model.set(GRB_IntParam_Threads, 1);
        
        // initital scheduling variables
        std::vector<GRBVar> S(n);
        std::vector<std::vector<GRBVar>> Y(n,std::vector<GRBVar>(n));
        for(int i = 0; i<n; i++){
            S[i] = model.addVar(0, T, 0, GRB_CONTINUOUS);
            for(int i2 = 0; i2<n; i2++){
                Y[i][i2] = model.addVar(0, 1, 0, GRB_BINARY);
            }
        }
        
        // assignment variables
        std::vector<std::vector<std::vector<GRBVar>>> X(n, std::vector<std::vector<GRBVar>>(s,std::vector<GRBVar>(r)));
        for(int i = 0; i<n; i++){
            for(int j = 0; j<s; j++){
                for(int k = 0; k<r; k++){
                    X[i][j][k] = model.addVar(0, 1, 0, GRB_BINARY);
                }
            }
        }
        
        // Training variables
        std::vector<std::vector<GRBVar>> H(r,std::vector<GRBVar>(s));
        std::vector<std::vector<GRBVar>> U(r,std::vector<GRBVar>(s));
        std::vector<std::vector<std::vector<GRBVar>>> V(r,std::vector<std::vector<GRBVar>>(s,std::vector<GRBVar>(n)));
        std::vector<std::vector<std::vector<GRBVar>>> W(r,std::vector<std::vector<GRBVar>>(s,std::vector<GRBVar>(n)));
        std::vector<std::vector<std::vector<GRBVar>>> Z(r,std::vector<std::vector<GRBVar>>(s,std::vector<GRBVar>(s)));
        for(int k = 0; k<r; k++){
            for(int j = 0; j<s; j++){
                H[k][j] = model.addVar(0, T, 0, GRB_CONTINUOUS);
                U[k][j] = model.addVar(0, 1, 0, GRB_BINARY);
                for(int i = 0; i<n; i++){
                    V[k][j][i] = model.addVar(0, 1, 0, GRB_BINARY);
                    W[k][j][i] = model.addVar(0, 1, 0, GRB_BINARY);
                }
                for(int j2 = 0; j2<s; j2++){
                    Z[k][j][j2] = model.addVar(0, 1, 0, GRB_BINARY);
                }
            }
        }
        
        // basic precedence constraints
        // start of activity i2 occurs after the completion of activity i if y[i][i2]=1
        for(int i = 0; i<n; i++){
            for(int i2 = 0; i2<n; i2++){
                if(i == i2){continue;}
                GRBLinExpr lhs = S[i]+data.durations[i];
                GRBLinExpr rhs = S[i2]+T*(1-Y[i][i2]);
                model.addConstr(lhs, GRB_LESS_EQUAL, rhs);
            }
        }
        // precedence restrictions
        for(int i = 0; i<n; i++){
            for(int i2 = 0; i2<data.num_tot_successors[i];i2++){
                int suc = data.index_tot_successors[i][i2];
                model.addConstr(Y[i][suc], GRB_EQUAL, 1);
                model.addConstr(Y[suc][i], GRB_EQUAL, 0);
            }
        }
        
        // basic assignment constraints
        // requirements are met
        for(int i = 0; i<n; i++){
            for(int j = 0; j<s; j++){
                GRBLinExpr rhs = data.skill_requirements[i][j];
                GRBLinExpr lhs = 0;
                for(int k = 0; k<r; k++){
                    lhs += X[i][j][k];
                }
                model.addConstr(lhs, GRB_EQUAL, rhs);
            }
        }
        // resource can not be assigned to parallel scheduled activities
        for(int i = 0; i<n; i++){
            for(int i2 = i+1; i2<n; i2++){
                for(int k = 0; k<r; k++){
                    GRBLinExpr lhs = 0;
                    GRBLinExpr rhs = 1+Y[i][i2]+Y[i2][i];
                    for(int j = 0; j<s; j++){
                        lhs += X[i][j][k]+X[i2][j][k];
                    }
                    model.addConstr(lhs, GRB_LESS_EQUAL, rhs);
                }
            }
        }
        
        // Training constraints
        // limited budget
        GRBLinExpr budget = data.tb;
        GRBLinExpr used_cost = 0;
        for(int k = 0; k<r; k++){
            for(int j = 0; j<s; j++){
                used_cost += U[k][j]*data.training_costs[j];
            }
        }
        model.addConstr(used_cost, GRB_LESS_EQUAL, budget);
        // link the V[k][j][i] and W[k][j][i] variable with S[i] and H[k][j]
        for(int k = 0; k<r; k++){
            for(int j = 0; j<s; j++){
                GRBLinExpr lhs_v = H[k][j]+data.training_duration[j];
                for(int i = 0; i<n; i++){
                    GRBLinExpr rhs_v = S[i]+T*(1-V[k][j][i]);
                    model.addConstr(lhs_v, GRB_LESS_EQUAL, rhs_v);
                    GRBLinExpr lhs_w = S[i]+data.durations[i];
                    GRBLinExpr rhs_w = H[k][j]+T*(1-W[k][j][i]);
                    model.addConstr(lhs_w, GRB_LESS_EQUAL, rhs_w);
                }
            }
        }
        // link V[k][j][i] and W[k][j][i] with U[k][j] and X[i][j][k]. A resource can not be trained while assigned
        for(int k = 0; k<r; k++){
            for(int j = 0; j<s; j++){
                for(int i = 0; i<n; i++){
                    GRBLinExpr lhs = 0;
                    GRBLinExpr rhs = 1 - (U[k][j] - V[k][j][i] - W[k][j][i]);
                    for(int j2 = 0; j2<s; j2++){
                        lhs += X[i][j2][k];
                    }
                    model.addConstr(lhs, GRB_LESS_EQUAL, rhs);
                }
            }
        }
        // link V[k][j][i] and W[k][j][i] with U[k][j]
        for(int k = 0; k<r; k++){
            for(int j = 0; j<s; j++){
                GRBLinExpr rhs = U[k][j];
                for(int i = 0; i<n; i++){
                    GRBLinExpr lhs = V[k][j][i] + W[k][j][i];
                    model.addConstr(lhs, GRB_LESS_EQUAL, rhs);
                }
            }
        }
        // skill aquisition, if trained, then it can be assigned
        for(int i = 0; i<n; i++){
            for(int k = 0; k<r; k++){
                for(int j = 0; j<s; j++){
                    GRBLinExpr lhs = X[i][j][k];
                    GRBLinExpr rhs = data.workforce[k][j]+V[k][j][i];
                    model.addConstr(lhs, GRB_LESS_EQUAL, rhs);
                }
            }
        }
        // limit training
        for(int k = 0; k<r; k++){
            for(int j = 0; j<s; j++){
                if (data.workforce[k][j] == 1) {
                    GRBLinExpr rhs = 1 - data.workforce[k][j];
                    GRBLinExpr lhs = U[k][j];
                    model.addConstr(U[k][j], GRB_EQUAL, rhs);
                }
            }
        }
        // training is not allowed to overlap with other training
        for(int k = 0; k<r; k++){
            for(int j = 0; j<s; j++){
                for(int j2 = j+1; j2<s; j2++){
                    //if(j == j2){continue;}
                    GRBLinExpr lhs_1 = H[k][j]+data.training_duration[j];
                    GRBLinExpr rhs_1 = H[k][j2]+M*(1-Z[k][j][j2])+M*(2-U[k][j]-U[k][j2]);
                    model.addConstr(lhs_1, GRB_LESS_EQUAL, rhs_1);
                    
                    GRBLinExpr lhs_2 = H[k][j2]+data.training_duration[j2];
                    GRBLinExpr rhs_2 = H[k][j]+M*(Z[k][j][j2])+M*(2-U[k][j]-U[k][j2]);
                    model.addConstr(lhs_2, GRB_LESS_EQUAL, rhs_2);
                }
            }
        }
        
        // objective function
        GRBLinExpr obj = S[n-1];
        model.setObjective(obj, GRB_MINIMIZE);
        model.set(GRB_DoubleParam_TimeLimit, timelimit);
        model.optimize();
        
        // output
        int status = model.get(GRB_IntAttr_Status);
        if (status == GRB_OPTIMAL) {sol.status = "OPT";}
        else if (status == GRB_INFEASIBLE){sol.status = "INF";}
        else if (status == GRB_TIME_LIMIT){sol.status = "FEAS";}
        
        if (model.get(GRB_IntAttr_SolCount) > 0)
        {
            // save information
            sol.makespan = model.get(GRB_DoubleAttr_ObjVal);
            
            sol.UB = model.get(GRB_DoubleAttr_ObjVal);      // Best feasible solution
            sol.LB = model.get(GRB_DoubleAttr_ObjBound);        // Best lower bound
            sol.CPU = model.get(GRB_DoubleAttr_Runtime);        // Runtime in seconds
            sol.GAP = model.get(GRB_DoubleAttr_MIPGap);         // Relative MIP gap
            
            //--------------------------------------------------
            // Trainings
            //--------------------------------------------------
            double total_trained = 0;
            for(int k = 0; k < r; k++)
            {
                for(int j = 0; j < s; j++)
                {
                    if(U[k][j].get(GRB_DoubleAttr_X) > 0.5){total_trained++;}
                }
            }
            // percentage trained
            sol.percentage_trained = (total_trained/(double)data.tb)*100;
            printf("");
            printf("");
            
            /*printf("\n=========================================\n");
            printf("           SOLUTION OVERVIEW\n");
            printf("=========================================\n");
            printf("Objective (Makespan): %.2f\n",
                   model.get(GRB_DoubleAttr_ObjVal));
            
            printf("\n-----------------------------------------\n");
            printf("Activity start times\n");
            printf("-----------------------------------------\n");
            for (int i = 0; i < n; i++)
            {
                printf("Activity %2d : Start = %8.2f\n",
                       i,
                       S[i].get(GRB_DoubleAttr_X));
            }
            
            printf("\n-----------------------------------------\n");
            printf("Resource assignments\n");
            printf("-----------------------------------------\n");
            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < s; j++)
                {
                    for (int k = 0; k < r; k++)
                    {
                        if (X[i][j][k].get(GRB_DoubleAttr_X) > 0.5)
                        {
                            printf("Activity %2d : Resource %2d performs Skill %2d\n",
                                   i, k, j);
                        }
                    }
                }
            }
            
            printf("\n-----------------------------------------\n");
            printf("Selected trainings\n");
            printf("-----------------------------------------\n");
            for (int k = 0; k < r; k++)
            {
                for (int j = 0; j < s; j++)
                {
                    if (U[k][j].get(GRB_DoubleAttr_X) > 0.5)
                    {
                        printf("Resource %2d trained on Skill %2d : Start = %8.2f  End = %8.2f\n",
                               k,
                               j,
                               H[k][j].get(GRB_DoubleAttr_X),
                               H[k][j].get(GRB_DoubleAttr_X) + data.training_duration[j]);
                    }
                }
            }
            
            printf("\n-----------------------------------------\n");
            printf("Activity sequencing (Y)\n");
            printf("-----------------------------------------\n");
            for (int i = 0; i < n; i++)
            {
                for (int i2 = 0; i2 < n; i2++)
                {
                    if (i == i2) continue;
                    
                    if (Y[i][i2].get(GRB_DoubleAttr_X) > 0.5)
                    {
                        printf("%2d -> %2d\n", i, i2);
                    }
                }
            }
            
            printf("\n-----------------------------------------\n");
            printf("Training before activity (V)\n");
            printf("-----------------------------------------\n");
            for (int k = 0; k < r; k++)
            {
                for (int j = 0; j < s; j++)
                {
                    for (int i = 0; i < n; i++)
                    {
                        if (V[k][j][i].get(GRB_DoubleAttr_X) > 0.5)
                        {
                            printf("Resource %2d Skill %2d before Activity %2d\n",
                                   k, j, i);
                        }
                    }
                }
            }
            
            printf("\n-----------------------------------------\n");
            printf("Activity before training (W)\n");
            printf("-----------------------------------------\n");
            for (int k = 0; k < r; k++)
            {
                for (int j = 0; j < s; j++)
                {
                    for (int i = 0; i < n; i++)
                    {
                        if (W[k][j][i].get(GRB_DoubleAttr_X) > 0.5)
                        {
                            printf("Activity %2d before Resource %2d Skill %2d\n",
                                   i, k, j);
                        }
                    }
                }
            }
            
            printf("\n-----------------------------------------\n");
            printf("Training order (Z)\n");
            printf("-----------------------------------------\n");
            for (int k = 0; k < r; k++)
            {
                for (int j = 0; j < s; j++)
                {
                    for (int j2 = 0; j2 < s; j2++)
                    {
                        if (j == j2) continue;
                        
                        if (Z[k][j][j2].get(GRB_DoubleAttr_X) > 0.5)
                        {
                            printf("Resource %2d : Skill %2d before Skill %2d\n",
                                   k, j, j2);
                        }
                    }
                }
            }
            
            printf("=========================================\n");*/
            
        }
        else if (model.get(GRB_IntAttr_SolCount) == 0){
            sol.status = "NOSOL";
            sol.UB = 9999;
            sol.LB = 9999;
            sol.GAP = 9999;
            sol.percentage_trained = 9999;
        }
        
    }
    catch (GRBException e)
    {
        std::cout << "Error code = " << e.getErrorCode() << std::endl;
        std::cout << e.getMessage() << std::endl;
    }
    
    
    return sol;
}
