#include "AlgoUtil.h"

std::vector<bool> algoUtil_int2boolVec(int val, size_t tam)
{
  std::vector<bool> res(tam, false);
  for (int i = tam - 1; i >= 0 && val; --i, val >>= 1)
    res[i] = val & 1;
  return res;
}
