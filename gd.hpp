#ifndef GD_HPP
#define GD_HPP

#include "graph.hpp"
#include "units.hpp"

#include <array>
#include <optional>
#include <utility>

std::pair<std::array<unsigned long, 4>,
          std::optional<std::pair<cupath, cupath>>>
gd(const graph &g, const demand &d, const CU &cu);

#endif // GD_HPP
