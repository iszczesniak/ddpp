#include "label.hpp"

bool
operator<=(const label &i, const label &j)
{
  return i.first <= j.first && i.second.includes(j.second);
}

bool
operator <= (const label_pair_t &a, const label_pair_t &b)
{
  return a.first <= b.first && a.second <= b.second;
}
