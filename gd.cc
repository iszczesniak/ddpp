#include "adaptive_units.hpp"
#include "graph.hpp"
#include "label.hpp"
#include "shared.hpp"
#include "units.hpp"
#include "utils.hpp"

#include <array>
#include <optional>
#include <queue>
#include <utility>

using namespace std;

template<typename C>
struct matrix
{
  std::vector<std::vector<C> > m_M;

  matrix(std::size_t n):
    m_M(n, std::vector<C>(n))
  {
  }

  const C &
  operator [](const vertex_pair_t &vp) const
  {
    assert(!(vp.first > vp.second));
    return m_M[vp.first][vp.second];
  };

  C &
  operator [](const vertex_pair_t &vp)
  {
    assert(!(vp.first > vp.second));
    return m_M[vp.first][vp.second];
  };
};

// The permanent labels type.
using permanent_t = matrix<std::vector<tnsp_t> >;

// The tentative labels type.
using tentative_t = matrix<std::set<tnsp_t> >;

// The priority queue element type.
using pqe_t = std::pair<COST, std::weak_ptr<tn_t> >;

// The priority queue type.
using pq_t = std::priority_queue<pqe_t, std::vector<pqe_t>,
                                 std::function<bool(const pqe_t &,
                                                    const pqe_t &)> >;

bool
has_better_or_equal(const std::vector<tnsp_t> &Svp,
                    const label_pair_t &j)
{
  for (auto const &e: Svp)
    {
      const label_pair_t &i = std::get<1>(*e);
      if (i <= j)
        return true;
    }

  return false;
}

bool
has_better_or_equal(const std::set<tnsp_t> &Qvp,
                    const label_pair_t &j)
{
  for (auto const &e: Qvp)
    {
      const label_pair_t &i = std::get<1>(*e);
      if (i <= j)
        return true;
    }

  return false;
}

void
discard_worse(tentative_t &Q, const vertex_pair_t &vp,
              const label_pair_t &j, unsigned long &Qc)
{
  auto &Qvp = Q[vp];

  for(auto iter = Qvp.begin(); iter != Qvp.end();)
    {
      const label_pair_t &i = std::get<1>(**iter);
      auto tmp = iter;
      ++iter;
      
      assert(i != j);

      if (j <= i)
        {
          Qvp.erase(tmp);
          --Qc;
        }
    }

}

void
relax(const graph &g, const permanent_t &S, tentative_t &Q,
      const edge &e, const vertex const_v, const label &const_l,
      const label &other_l, const tnsp_t &tnsp, pq_t &pq,
      const int &ncu, unsigned long &Qc)
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

      if (has_better_or_equal(S[cvp], clp) ||
          has_better_or_equal(Q[cvp], clp))
        continue;

      // Make sure that e has not been already used.
      const tn_t *ptr = tnsp.get();
      for(; ptr != nullptr; ptr = std::get<4>(*ptr).get())
        if (std::get<2>(*ptr) == e)
          break;

      // Edge e is not used when we reached the tree root.
      if (ptr == nullptr)
        {
          discard_worse(Q, cvp, clp, Qc);
          auto sp = make_shared<tn_t>(cvp, clp, e, swapped, tnsp);
          Q[cvp].insert(sp);
          ++Qc;
          COST l1 = clp.first.first;
          COST l2 = clp.second.first;
          COST c1 = l1 * adaptive_units<COST>::units(ncu, l1);
          COST c2 = l2 * adaptive_units<COST>::units(ncu, l2);
          pq.push({c1 + c2, sp});
        }
    }
}

pair<array<unsigned long, 4>,
     std::optional<std::pair<cupath, cupath>>>
gd(const graph &g, const demand &d, const CU &cu)
{
  std::optional<std::pair<cupath, cupath> > result;

  vertex src = d.first.first;
  vertex dst = d.first.second;

  // The number of contiguous units.
  int ncu = d.second;

  // The permanent labels.
  permanent_t S(num_vertices(g));
  // The tentative labels.
  tentative_t Q(num_vertices(g));

  // The priority queue.
  pq_t pq([](const pqe_t &a, const pqe_t &b)
          {return !(a.first < b.first);});

  // The boot label.
  auto blsp = std::make_shared<tn_t>(pair(src, src),
                                     pair(label(0, cu), label(0, cu)),
                                     edge(), false, nullptr);

  // Boot the search with the root of the search tree.
  pq.push({0, blsp});
  Q[pair(src, src)].insert(blsp);

  // Count of permanent labels.
  unsigned long Sc = 0;
  // Count of tentative labels.
  unsigned long Qc = 0;

  // The max memory words used.
  unsigned long mmwu = 0;
  // The max priority queue count, when mmwu was max.
  unsigned long mpqc = 0;
  // The max permanent labels count, when mmwu was max.
  unsigned long msc = 0;
  // The max tentative labels count, when mmwu was max.
  unsigned long mqc = 0;

  while(!pq.empty())
    {
      // The number of priority queue memory words.  Each queue
      // element has the cost of 8 bytes (one 64-bit word), and a weak
      // pointer of 8 bytes (one 64-bit word).  We account for the
      // memory taken by the management structure of the weak pointer
      // when we account for the memory taken by the shared pointers
      // of the tentative labels.
      unsigned long pqmw = 2 * pq.size();
      // For each permanent label we need:
      // * 4 words (a most, usually 2 words) for a shared pointer, so
      //   so that the label is stored in a vector,
      // * 1 word for a vertex pair,
      // * 4 words for a label pair (2 * 2: 1 for COST, 1 for CU),
      // * 2 words for an edge,
      // * 4 words for a shared pointer to the parent tree node.
      unsigned long Smw = 15 * Sc;
      // For each tentative label we need the same number of words as
      // for a permanent label.
      unsigned long Qmw = 15 * Qc;
      // Memory words used.
      unsigned mwu = pqmw + Smw + Qmw;
      
      if (mmwu < mwu)
        {
          mmwu = mwu;
          mpqc = pq.size();
          msc = Sc;
          mqc = Qc;
        }

      // Here we get a copy of a shared_ptr.
      auto pqe = pq.top();
      pq.pop();
      auto tnsp = pqe.second.lock();

      // We don't care about a removed label.
      if (tnsp == nullptr)
        continue;

      const tn_t &tn = *tnsp;

      // The vertex pair of the tree node.
      const vertex_pair_t &vp = std::get<0>(tn);

      // We remove the shared_ptr from the tentative set.
      auto num = Q[vp].erase(tnsp);
      assert(num == 1);
      // We push_back to have the labels sorted according to cost.
      S[vp].push_back(tnsp);

      // Modify the counts.
      --Qc;
      ++Sc;

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
      if (v1 != dst)
        for(const auto &e: make_iterator_range(out_edges(v1, g)))
          relax(g, S, Q, e, v2, l2, l1, tnsp, pq, ncu, Qc);

      // We are leaving vertex v2.
      if (v2 != dst)
        for(const auto &e: make_iterator_range(out_edges(v2, g)))
          relax(g, S, Q, e, v1, l1, l2, tnsp, pq, ncu, Qc);
    }

  return make_pair(array<unsigned long, 4>{mmwu, mpqc, msc, mqc},
                   result);
}
