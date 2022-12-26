#include <boost/graph/filtered_graph.hpp>

#include "graph.hpp"

#include "adaptive_units.hpp"
#include "generic_dijkstra.hpp"
#include "generic_label.hpp"
#include "generic_permanent.hpp"
#include "generic_tentative.hpp"
#include "generic_tracer.hpp"
#include "label_creator.hpp"
#include "utils.hpp"

#include <optional>
#include <set>
#include <utility>

using namespace std;

template<typename Graph>
optional<cupath>
search(const Graph &g, const demand &d, const CU &cu)
{
  vertex src = d.first.first;
  vertex dst = d.first.second;
  // The number of contiguous units.
  int ncu = d.second;

  assert (src != dst);

  // The generic permanent solution type.
  using per_type = generic_permanent<Graph, COST, CU>;
  // The generic tentative solution type.
  using ten_type = generic_tentative<Graph, COST, CU>;

  // The permanent and tentative solutions.
  per_type P(boost::num_vertices(g));
  ten_type T(boost::num_vertices(g));
  // The label we start the search with.
  generic_label<Graph, COST, CU> l(0, CU(cu), edge(), src);
  // The creator of the labels.
  label_creator<COST> c(ncu);

  // Run the search.
  dijkstra(g, l, P, T, c, dst);

  // The tracer.
  generic_tracer<Graph, cupath, per_type, CU> t(g, ncu);
  // Get the path.
  auto op = trace(P, dst, l, t);

  if (op)
    {
      // The length of the path found.
      auto dist = get_path_length(g, op.value().second);
      // The path CU.
      auto &pcu = op.value().first;

      // Get the number of units required.
      int units = adaptive_units<COST>::units(ncu, dist);

      // First-fit spectrum allocation policy.
      pcu = CU(pcu.min(), pcu.min() + units);
    }

  return op;
}

std::optional<std::pair<cupath, cupath> >
ee(const graph &g, const demand &d, const CU &cu)
{
  std::optional<std::pair<cupath, cupath> > result;

  // We have to run the algorithm twice: first without filtering,
  // second with filtering.
  auto first = search(g, d, cu);

  if (first)
    {
      const auto &p1 = first.value();

      // Excluded edges.
      set<edge> ee;

      for(const auto &e: p1.second)
        ee.insert(e);

      // The filtered graph type.
      using fg_type =
        boost::filtered_graph<graph,
                              boost::is_not_in_subset<set<edge>>>;

      fg_type fg(g, ee);

      auto second = search(fg, d, cu);

      if (second)
        {
          const auto &p2 = second.value();

          // Let's make sure that the first path is not of a greater
          // cost than the second path.
          assert(get_cost(g, p1) <= get_cost(g, p2));

          // Let's make sure that the two paths have no mutual edges.
          for(const auto &e1: p1.second)
            for(const auto &e2: p2.second)
              assert(e1 != e2);

          return make_pair(p1, p2);
        }
    }

  return {};
}
