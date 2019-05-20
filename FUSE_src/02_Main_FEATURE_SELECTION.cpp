///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////                            Feature Selection as a One-Player Game                         //////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    This code is an implementation of the algorithm described in (https://hal.inria.fr/inria-00484049/document)    //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The feature selection algorithm is run on the training set "dataset.dat.                                          //
// The reading function can be modified in dataset.cpp                                                               //
// The algorithm is based on bandit reinforcement learning, and is an improved version of UCT                        //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Created : 2018-03-12                                                                                              //
// Updated : 2018-05-01                                                                                              //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#include <iostream>         //cin & cout
#include <algorithm>        //min, max, fill, find...
#include <fstream>          //files write and read
#include <string>           //string
#include <map>
#include <vector>           //vector
#include <iomanip>          //set precision & set width
#include <bitset>           //for binary numbers manipulation
#include <unordered_map>    //for hash function
#include <chrono>           //to record simulation time
#include <windows.h>        //get key
using namespace std;

//libraries
#include <boost/dynamic_bitset.hpp>  //for binary vectors (couldn't use std::bitset because the size had to be defined at compilation time)

//including functions
#include "mainf.h"
#include "dataset.h"
#include "update.h"
#include "print.h"

//including classes
#include "class_Params.h"   //Contains all the important simulation parameters
#include "class_Node.h"     //Contains all the parameters of a node
#include "class_Tree.h"     //Pair of a Node vector and an associated hash function to quickly find nodes address - in O(1)



void Run_feature_selection(Params &params)
{
    if (params.Nt<1000) {cout << endl << "   Error: Number if iteration should be at least 1000" << endl << endl; exit(1);} //or the output files will be messed up
    cout << fixed << setprecision(4);

    //variables
    int f = params.L[0].size() - 1;                //Number of features
    boost::dynamic_bitset<> F(f+1);                //F[f+1] Feature subset with F[fi]=0 if feature not used, + the stopping feature
    Tree T(f+1);                                   //Tree T is a Dynamic array of node + with a hash function to find its element faster, f+1 for the stopping feature
    vector < pair <double, int> > gRAVE(f*2+2);    //Global RAVE score, pair of <RAVE_score, Nb of calc involved>, all are initialized to zero.
                                                   //(the first f index are for each features, and the f+1 remaining are for the stopping feature at each feature subset size)
    Variables variables = Variables();




    cout << "     n = " << params.L.size() << " examples" << endl;
    cout << "     f = " << params.L[0].size()-1 << " features" << endl << endl << endl << endl;
    cout << "  Running feature selection for " << setw(3) << std::setfill('0') << (params.Nt / 1000) << " " << setw(3) << std::setfill('0') << (params.Nt % 1000) << " iterations ..." << endl << endl;


    int duree = (int) ((double)params.L.size()*(double)params.L[0].size()*(double)params.m*(double)params.Nt / 334000000.0);
    int heures=int(duree / 3600);
    int minutes=int((duree % 3600) / 60);
    int secondes=int(((duree % 3600) % 60));

    cout  << endl;



    int it0 = 0;

    //======================================================= Running the simulations =======================================================//

    Run_FUSE(T, gRAVE, params, F, variables, it0);


   //============================================= Terminate the simulation and print results =============================================//

    print_results(T, gRAVE, variables, params);




    ofstream output_file ("BigResults.txt", ios_base::app);
    if (output_file.is_open())
    {
        FILE* pFile = freopen( "BigResults.txt", "a", stdout );

        boost::dynamic_bitset<> F = T.N[0].sub_F;
        vector<int> selected_features;
        vector<int> path_address;
        int f = F.size()-1;
        int node_address = 0;
        int child_address;
        map <double, int>  TF;
        int fi;
        int tf;
        cout << "[";
        while (!F[f])
        {
            TF.clear();
            TF.insert({(double)T.N[node_address].fs[1], f});
            double avfi = 0;
            for (int i=0; i<f ;i++)
            {
                child_address = T.N[node_address].address_f[i];
                if (child_address != -1)
                {
                    avfi = T.N[child_address].av_F;
                    //avfi = max(T.N[child_address].av_F,T.N[child_address].fs[1]);
                    TF.insert({avfi, i});
                }
            }

            //choosing the most visited child node
            tf = TF.rbegin()->first;
            fi = TF.rbegin()->second;
            selected_features.push_back(fi);
            if (fi!=f)
            {
                node_address = T.N[node_address].address_f[fi];
                path_address.push_back(node_address);
                cout << " " << setw(3) << std::setfill('0') << fi << " ";
            }
            F[fi] = 1;
        }
        cout << "]; " << setprecision(4) << T.N[node_address].fs[1] << ";" << endl;

        /*
        cout << " [";
        for (int i=0; i<path_address.size(); i++)
        {
            cout << " " << setprecision(4) << T.N[path_address[i]].fs[1] << " ";
        }
        cout << "]" << endl << endl;
        */


        output_file.close();
    }
    else cout << "Unable to open Results.txt" << endl;
    freopen( "CON", "w", stdout );

}






void    Run_FUSE(Tree &T, vector <pair<double,int>> &gRAVE, Params &params, boost::dynamic_bitset<> &F, Variables &variables, int it0)
{
    int it;
    double V;
    int depth;
    auto t_start = std::chrono::high_resolution_clock::now(); //record the starting time



    //======================================================= Running the simulations =======================================================//

    for (it = it0; it < params.Nt; it++)
    {
        /*
        if(GetKeyState('J') & 0x8000) //check if high-order bit is set
        {
            break;
        }
        */

        if (it%2000 == 0) {cout << endl << "    it = " << setw(3) << std::setfill('0') << (it / 1000) << " " << setw(3) << std::setfill('0') << (it % 1000);
                         cout << " / " << setw(3) << std::setfill('0') << (params.Nt / 1000) << " " << setw(3) << std::setfill('0') << (params.Nt % 1000) << endl;}
        //if (it%20000== 0) cout << "       You can pause the search any time by pressing 'j'" << endl;

        if (it == 31)
        {
            //cout << "debug" << endl;
        }

        if ((it%2000 == 0) && (true))
        {
            Node N_best_path_Av = Find_Best_Node_Path_Av(T, params);
            variables.reward_V2.push_back(N_best_path_Av.fs[1]);
            variables.Depth2.push_back(N_best_path_Av.F_size);
        }

        //reinitialize the feature subset to empty and clear the path;
        F = F.reset();    //start from empty feature subset after each iteration
        depth = 0;
        for (int i=0;i<T.N.size();i++)
        {
            T.N[i].tobe_updated = 0;   //reset the values to zero for next backpropagation
            T.N[i].already_updated_score = 0;
            T.N[i].weight = 0;
        }
        T.N[0].weight = 1.0;

        //Explore the tree (until it chose final feature
        V = iterate(T, gRAVE, F, params, depth);
        variables.reward_V.push_back(V);
        variables.Depth.push_back(depth);
    }



    //recorde the time
    auto t_finish = std::chrono::high_resolution_clock::now(); //record the finishing time
    std::chrono::duration<double> t_elapsed = t_finish - t_start; //get the elapsed time
    variables.time += (int)t_elapsed.count();
    it0 = it;


    //print the current best node
    cout << endl << endl;
    Node N_best_final = Find_Best_Node_Final(T);
    print_Node(T, N_best_final);
    Node N_best_path = Find_Best_Node_Path(T, params);
    print_Node(T, N_best_path);
    Node N_best_path_avfs = Find_Best_Node_Path_Avfs(T, params);
    print_Node(T, N_best_path_avfs);
    Node N_best_path_av = Find_Best_Node_Path_Av(T, params);
    print_Node(T, N_best_path_av);
    //print_other_nodes(T, N_best_path_av.sub_F, gRAVE);


    //ask if the user wants to pursue the search
    cout << endl << endl;
    bool bool_continue = false;
    if (bool_continue)
    {
        params.Nt += it0;
        Run_FUSE(T, gRAVE, params, F, variables, it0);
    }
    else params.Nt = it0;

}




bool  get_answer()
{
    cout << " Do you want to continue the search ? (press \'Enter\' or \'Y\' for yes, \'N\' or \'Q\' for no)" << endl << "     ";
    string Answer;
    getline( cin, Answer );
    if (  (Answer.empty()) || ((Answer[0] == 'y') || (Answer[0] == 'Y'))  )
    {
        return true;
    }
    else if (  ((Answer[0] == 'n') || (Answer[0] == 'N')) || ((Answer[0] == 'Q')  || (Answer[0] == 'q'))  )
    {
        return false;
    }
    else
    {
        cout << endl << "     Sorry, I did not understand what you want." << endl << endl << endl;
        return get_answer();
    }
}
