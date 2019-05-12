#ifndef GD_HPP
#define GD_HPP

#include "graph.hpp"
#include "units.hpp"

#include <optional>
#include <tuple>
#include <utility>

std::tuple<unsigned long, unsigned long, unsigned long, unsigned long,
           std::optional<std::pair<cupath, cupath> > >
gd(const graph &g, const demand &d, const CU &cu);

#endif // GD_HPP
