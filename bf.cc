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

// The priority queue element type.
using pqe_t = std::pair<COST, std::shared_ptr<tn_t> >;

// The priority queue type.
using pq_t = std::priority_queue<pqe_t, std::vector<pqe_t>,
                                 std::function<bool(const pqe_t &,
                                                    const pqe_t &)> >;

void
replenish(const graph &g, const edge &e, const vertex const_v,
          const label &const_l, const label &other_l,
          const tnsp_t &tnsp, pq_t &pq, const std::size_t &ncu)
{
  // The new vertex - target of edge e.
  vertex nv = target(e, g);
  // The edge cost.
  auto ec = boost::get(boost::edge_weight, g, e);
  // The edge SU.
  const SU &e_su = boost::get(boost::edge_su, g, e);
  // The new cost.
  auto nc = other_l.first + ec;
  // The new candidate SU.
  SU n_su = intersection(SU{other_l.second}, e_su);
  n_su.remove(adaptive_units<COST>::units(ncu, nc));

  // Iterate over new candidate CUs.
  for(const auto &n_cu: n_su)
    {
      // Candidate vertex pair.
      vertex_pair_t cvp(const_v, nv);
      // Candidate label pair.
      label_pair_t clp(const_l, label(nc, n_cu));

      bool swapped = false;
      if (cvp.first > cvp.second)
        {
          std::swap(cvp.first, cvp.second);
          std::swap(clp.first, clp.second);
          swapped = true;
        }
      else if (cvp.first == cvp.second && !(clp.first <= clp.second))
        {
          std::swap(clp.first, clp.second);
          swapped = true;
        }

      // Make sure that e has not been already used.
      const tn_t *ptr = tnsp.get();
      for(; ptr != nullptr; ptr = std::get<4>(*ptr).get())
        if (std::get<2>(*ptr) == e)
          break;

      // Edge e is not used when we reached the tree root.
      if (ptr == nullptr)
        {
          auto sp = make_shared<tn_t>(cvp, clp, e, swapped, tnsp);
          COST l1 = clp.first.first;
          COST l2 = clp.second.first;
          COST c1 = l1 * adaptive_units<COST>::units(ncu, l1);
          COST c2 = l2 * adaptive_units<COST>::units(ncu, l2);
          pq.push({c1 + c2, sp});
        }
    }
}

std::optional<std::pair<cupath, cupath> >
bf(const graph &g, const demand &d, const CU &cu)
{
  std::optional<std::pair<cupath, cupath> > result;

  const vertex &src = d.first.first;
  const vertex &dst = d.first.second;
  // The number of contiguous units.
  const unsigned &ncu = d.second;

  // The priority queue.
  pq_t pq([](const pqe_t &a, const pqe_t &b)
          {return !(a.first < b.first);});

  // The boot label.
  auto blsp = std::make_shared<tn_t>(pair(src, src),
                                     pair(label(0, cu), label(0, cu)),
                                     edge(), false, nullptr);

  // Boot the search with the root of the search tree.
  pq.push({0, blsp});

  while(!pq.empty())
    {
      // Here we get a copy of a shared_ptr.
      auto pqe = pq.top();
      pq.pop();
      const tnsp_t &tnsp = pqe.second;
      const tn_t &tn = *tnsp;

      // The vertex pair of the tree node.
      const vertex_pair_t &vp = std::get<0>(tn);

      // Vertexes.
      const auto & [v1, v2] = vp;
      assert(v1 <= v2);

      if (v1 == dst && v2 == dst)
        {
          result = trace(g, tn, ncu);
          break;
        }

      // Labels.
      const auto & [l1, l2] = std::get<1>(tn);

      // We are leaving vertex v1.
      for(const auto &e: make_iterator_range(out_edges(v1, g)))
        replenish(g, e, v2, l2, l1, tnsp, pq, ncu);

      // We are leaving vertex v2.
      for(const auto &e: make_iterator_range(out_edges(v2, g)))
        replenish(g, e, v1, l1, l2, tnsp, pq, ncu);
    }

  return result;
}
