#ifndef LABEL_HPP
#define LABEL_HPP

#include "graph.hpp"

#include <utility>

using vertex_pair_t = std::pair<vertex, vertex>;

// The label type.
using label = std::pair<COST, CU>;

// The label pair.
using label_pair_t = std::pair<label, label>;

bool
operator<=(const label &i, const label &j);

bool
operator<=(const label_pair_t &a, const label_pair_t &b);

#endif // LABEL_HPP
