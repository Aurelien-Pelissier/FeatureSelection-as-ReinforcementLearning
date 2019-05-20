#ifndef INCLUDED_QUICKH
#define INCLUDED_QUICKH


#include <vector>




//main
std::vector<int>             quick_select(const int &k, std::vector<std::pair<float, int>> &Arr0, const bool &make_copy);
std::vector<int>             select(const int &k0, std::vector<std::pair<float, int>> &Arr, int k, int p, int r);
int                          partitions(std::vector<std::pair<float, int>> &Arr, int p, int r);


#endif
