#include "routing.hpp"

// I need to include this file here, because it introduces the
// specializations of the "target" and "get" functions, which have to
// be known before the standard_dijkstra.hpp is included.
#include <boost/graph/filtered_graph.hpp>

#include "adaptive_units.hpp"
#include "bf.hpp"
#include "ee.hpp"
#include "gd.hpp"
#include "graph.hpp"
#include "stats.hpp"
#include "units.hpp"
#include "utils.hpp"

#include <boost/range.hpp>

#include <algorithm>
#include <climits>
#include <chrono>
#include <list>
#include <map>
#include <optional>
#include <set>
#include <tuple>

using namespace std;

// Another routing algorithms to use.
set<routing::rt_t> routing::m_algs;

optional<cupp>
routing::set_up(graph &g, const demand &d)
{
  vertex src = d.first.first;

  // The maximal total number of units we found.
  unsigned nou = 0;

  // Itereate over the out edges of the src vertex.
  for(const auto &e: make_iterator_range(out_edges(src, g)))
    // The total number of units available on the edge.
    nou = std::max(nou, boost::get(boost::edge_nou, g, e));

  return set_up(g, d, CU(0, nou));
}

optional<cupp>
routing::set_up(graph &g, const demand &d, const CU &cu)
{
  vertex src = d.first.first;
  vertex dst = d.first.second;

  assert (src != dst);

  // The path found to be established.
  optional<cupp> p;

  // The algorithms to use.
  bool gd = m_algs.count(rt_t::gd);
  bool bf = m_algs.count(rt_t::bf);
  bool ee = m_algs.count(rt_t::ee);

  if (gd)
    {
      // The generic Dijkstra result.
      p = search(g, d, cu, rt_t::gd);

      // Corroborate the generic Dijkstra results with the brute force
      // results.
      if (bf)
        {
          // The brute force result.
          auto pp = search(g, d, cu, rt_t::bf);

          if (!(!p && !pp ||
                get_cost(g, p.value()) == get_cost(g, pp.value())))
            abort();
        }

      // We run this research only to report performance, we don't use
      // its results.
      if (ee)
        search(g, d, cu, rt_t::ee);
    }
  else
    {
      assert(ee);
      // The edge exclusion results.
      p = search(g, d, cu, rt_t::ee);
    }

  if (p)
    set_up(g, p.value());

  return p;
}

optional<cupp>
routing::search(graph &g, const demand &d, const CU &cu, rt_t rt)
{
  using tp_t = chrono::time_point<chrono::high_resolution_clock>;

  pair<array<unsigned long, 4>, optional<cupp>> p;

  tp_t t0 = std::chrono::system_clock::now();

  switch (rt)
    {
    case rt_t::gd:
      p = gd(g, d, cu);
      break;

    case rt_t::bf:
      p = make_pair(array<unsigned long, 4>{0, 0, 0, 0},
                    bf(g, d, cu));
      break;

    case rt_t::ee:
      p = make_pair(array<unsigned long, 4>{0, 0, 0, 0},
                    ee(g, d, cu));
      break;

    default:
      abort();
    }

  tp_t t1 = std::chrono::system_clock::now();
  chrono::duration<double> dt = t1 - t0;

  stats::get().algo_perf(rt, dt.count(), d.second, p);

  return p.second;
}

void
routing::tear_down(graph &g, const cupath &p)
{
  boost::property_map<graph, boost::edge_su_t>::type
    sm = get(boost::edge_su_t(), g);

  // Iterate over the edges of the path.
  for(const auto &e: p.second)
    sm[e].insert(p.first);
}

void
routing::tear_down(graph &g, const cupp &p)
{
  tear_down(g, p.first);
  tear_down(g, p.second);
}

void
routing::set_up(graph &g, const cupath &p)
{
  boost::property_map<graph, boost::edge_su_t>::type
    sm = get(boost::edge_su_t(), g);

  for(const auto &e: p.second)
    sm[e].remove(p.first);
}

void
routing::set_up(graph &g, const cupp &p)
{
  set_up(g, p.first);
  set_up(g, p.second);
}

string
routing::to_string(rt_t rt)
{
  static const map<rt_t, string> t2s
  {{rt_t::gd, "gd"},
   {rt_t::bf, "bf"},
   {rt_t::ee, "ee"}};
  auto i = t2s.find(rt);
  assert(i != t2s.end());
  return i->second;
}

void
routing::add_algorithm(const rt_t rt)
{
  m_algs.insert(rt);
}
