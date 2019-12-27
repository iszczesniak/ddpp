#include "graph.hpp"

#include "adaptive_units.hpp"
#include "generic_dijkstra.hpp"
#include "generic_label_creator.hpp"
#include "generic_label.hpp"
#include "generic_permanent.hpp"
#include "generic_tentative.hpp"
#include "generic_tracer.hpp"

#include <optional>
#include <utility>

using namespace std;

optional<cupath>
search(const graph &g, const demand &d, const CU &cu)
{
  vertex src = d.first.first;
  vertex dst = d.first.second;
  // The number of contiguous units.
  int ncu = d.second;

  assert (src != dst);

  // The generic permanent solution type.
  using per_type = generic_permanent<graph, COST, CU>;
  // The generic tentative solution type.
  using ten_type = generic_tentative<graph, COST, CU>;

  // The permanent and tentative solutions.
  per_type P(boost::num_vertices(g));
  ten_type T(boost::num_vertices(g));
  // The label we start the search with.
  generic_label<graph, COST, CU> l(0, CU(cu), edge(), src);
  // The creator of the labels.
  generic_label_creator<graph, COST, CU> c(g, ncu);
  // Run the search.
  dijkstra(g, P, T, l, c, dst);
  // The tracer.
  generic_tracer<graph, cupath, per_type, CU> t(g, ncu);
  // Get the path.
  auto op = trace(P, dst, l, t);

  return op;
}

std::optional<std::pair<cupath, cupath> >
ee(const graph &g, const demand &d, const CU &cu)
{
  std::optional<std::pair<cupath, cupath> > result;

  // We have to run the algorithm twice: first without filtering, then
  // with filtering.
  auto first = search(g, d, cu);

  if (first)
    {
      auto second = search(g, d, cu);

      if (second)
        return make_pair(first.value(), second.value());
    }

  return {};
}
