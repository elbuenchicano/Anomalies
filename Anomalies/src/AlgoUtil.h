#ifndef ALGOUTIL_H
#define ALGOUTIL_H

#include <vector>
#include <assert.h>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//Metric units
class Metric_units {
public:
  double tp_ = 0, tn_ = 0, fp_ = 0, fn_ = 0;
  Metric_units() {
    ptr = &tp_;
  }
  double & operator [](int pos) {
    assert(pos >= 0 && pos < 4);
    return ptr[pos];
  }
private: //if i want create a c construct 
  double * ptr;
};

//..............................................................................
double supp_Precision         (Metric_units uni);

//..............................................................................
double supp_Recall            (Metric_units uni);

//..............................................................................
double supp_FalsePositiveRate (Metric_units uni);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//t tipe must have operator < and == overloaded, vec must be sorted
template <class t>
bool algoUtil_bin_search(std::vector<t> & vec, t val, int & pos,
  int ini = 0,
  int fin = 1 << 31) {
  if (fin >= vec.size())
    fin = static_cast<int>(vec.size() - 1);

  for (int m = (ini + fin) >> 1; ini <= fin; m = (ini + fin) >> 1) {
    if (vec[m] == val) {
      pos = m;
      return true;
    }
    if (vec[m] > val)
      fin = m - 1;
    else
      ini = m + 1;
  }
  return false;
}

//returns a boolvector in binary representation
std::vector<bool> algoUtil_int2boolVec(int, size_t);




#endif // !ALGOUTIL_H
