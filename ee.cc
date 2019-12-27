#include "adaptive_units.hpp"
#include "graph.hpp"
#include "label.hpp"
#include "shared.hpp"
#include "units.hpp"
#include "utils.hpp"

#include <optional>
#include <queue>
#include <utility>

using namespace std;

std::optional<std::pair<cupath, cupath> >
ee(const graph &g, const demand &d, const CU &cu)
{
  std::optional<std::pair<cupath, cupath> > result;

  const vertex &src = d.first.first;
  const vertex &dst = d.first.second;
  // The number of contiguous units.
  const unsigned &ncu = d.second;

  return result;
}
