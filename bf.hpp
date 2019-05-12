#ifndef BF_HPP
#define BF_HPP

#include "graph.hpp"
#include "units.hpp"

#include <optional>
#include <utility>

std::optional<std::pair<cupath, cupath> >
bf(const graph &g, const demand &d, const CU &cu);

#endif // BF_HPP
