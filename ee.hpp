#ifndef EE_HPP
#define EE_HPP

#include "graph.hpp"
#include "units.hpp"

#include <optional>
#include <utility>

std::optional<std::pair<cupath, cupath> >
ee(const graph &g, const demand &d, const CU &cu);

#endif // EE_HPP
