#define BOOST_TEST_MODULE ideas

#include "graph.hpp"
#include "utils.hpp"

#include <boost/test/unit_test.hpp>

#include <optional>
#include <queue>
#include <utility>

using namespace std;

// Tree node type declared.
struct tnt;

// Tree node type defined.
struct tnt: std::tuple<COST, std::pair<vertex, vertex>,
                       std::pair<COST, COST>, edge,
                       std::shared_ptr<tnt> >
{
  using base = std::tuple<COST, std::pair<vertex, vertex>,
                          std::pair<COST, COST>, edge,
                          std::shared_ptr<tnt> >;

  using base::base;

  bool
  operator <(const tnt &a) const
  {
    return std::get<0>(*this) < std::get<0>(a);
  }
};

// A priority queue element type.
using pqet = std::shared_ptr<tnt>;

// The priority queue type.
using pqt = std::priority_queue<pqet, std::vector<pqet>,
                                std::function<bool(const pqet &,
                                                   const pqet &)> >;
void
relax(const graph &g, 
      std::vector<std::vector<std::pair<COST, COST> > > &known,
      const edge &e, vertex nv1, COST nc1, COST c2,
      const pqet &pqe, pqt &pq)
{
  // The target of edge e.
  vertex nv2 = target(e, g);
  // The edge cost.
  auto ec = boost::get(boost::edge_weight, g, e);
  // The candidate cost.
  auto nc2 = c2 + ec;

  if (nv1 > nv2)
    {
      std::swap(nv1, nv2);
      std::swap(nc1, nc2);
    }
  else if (nv1 == nv2 && nc1 > nc2)
    std::swap(nc1, nc2);

  if (known[nv1][nv2].first < nc1 || known[nv1][nv2].second < nc2)
    return;

  // Make sure that e has not been already used.
  const tnt *ptr = pqe.get();

  for(; ptr != nullptr; ptr = std::get<4>(*ptr).get())
    if (std::get<3>(*ptr) == e)
      break;

  // Edge e is not used when we reached the tree root.
  if (ptr == nullptr)
    {
      known[nv1][nv2] = {nc1, nc2};
      pq.push(std::make_shared<tnt>(nc1 + nc2,
                                    std::pair(nv1, nv2),
                                    std::pair(nc1, nc2),
                                    e, pqe));
    }
}

auto
gd_search(vertex src, vertex dst, const graph &g)
{
  std::optional<std::pair<path, path> > result;

  // That's the maximal pair.
  std::pair<COST, COST> mp(std::numeric_limits<int>::max(),
                           std::numeric_limits<int>::max());

  std::vector<std::vector<std::pair<COST, COST> > >
    known(num_vertices(g),
          std::vector<std::pair<COST, COST> >(num_vertices(g), mp));

  // That's our priority queue that does the sorting based on the cost
  // of the tree node.  We use operators <, and ! to sort in the
  // ascending order.
  pqt pq([](const pqet &a, const pqet &b){return !(*a < *b);});

  // Boot the search with the root of the search tree.
  pq.push(std::make_shared<tnt>(0,
                                std::pair<vertex, vertex>(src, src),
                                std::pair<COST, COST>(0, 0),
                                edge(), nullptr));

  while(!pq.empty())
    {
      // Here we get a copy of a shared_ptr.
      auto pqe = pq.top();
      pq.pop();
      const tnt &tn = *pqe;
      // Vertexes.
      const auto [v1, v2] = std::get<1>(tn);
      // Costs.
      const auto [c1, c2] = std::get<2>(tn);

      if (v1 == dst && v2 == dst)
        {
          path p1, p2;

          // Trace back the paths.  We start at the current tree node,
          // i.e., tnp, and end at the root (which has nullptr as the
          // parent node).
          for(const tnt *ptr = pqe.get();
              std::get<4>(*ptr).get() != nullptr;
              ptr = std::get<4>(*ptr).get())
            {
              // This is the edge that we add.
              const edge &e = std::get<3>(*ptr);
              vertex t = target(e, g);

              if (!p1.empty() && source(p1.front(), g) == t)
                p1.push_front(e);
              else if (!p2.empty() && source(p2.front(), g) == t)
                p2.push_front(e);
              else
                {
                  assert(p1.empty() || p2.empty());

                  if (p1.empty())
                    p1.push_front(e);
                  else if (p2.empty())
                    p2.push_front(e);
                }
            }

          if (get_path_length(g, p1) <= get_path_length(g, p2))
            result = pair(p1, p2);
          else
            result = pair(p2, p1);
        }

      // We are leaving vertex v1.
      for(const auto &e: make_iterator_range(out_edges(v1, g)))
        relax(g, known, e, v2, c2, c1, pqe, pq);

      // We are leaving vertex v2.
      for(const auto &e: make_iterator_range(out_edges(v2, g)))
        relax(g, known, e, v1, c1, c2, pqe, pq);
    }

  return result;
}

// -------------------------------------------------------------------
//
//     (1)
//     /|\
// e1 4 | 1 e2
//   /e3|  \
// (0)  1  (3)
//   \  |  /
// e4 1 | 5 e5
//     \|/
//     (2)
//
BOOST_AUTO_TEST_CASE(ideas_1)
{
  graph g(4);
  auto p1 = boost::add_edge(0, 1, g);
  auto p2 = boost::add_edge(1, 3, g);
  auto p3 = boost::add_edge(1, 2, g);
  auto p4 = boost::add_edge(0, 2, g);
  auto p5 = boost::add_edge(2, 3, g);

  assert(p1.second);
  assert(p2.second);
  assert(p3.second);
  assert(p4.second);
  assert(p5.second);

  const edge &e1 = p1.first;
  const edge &e2 = p2.first;
  const edge &e3 = p3.first;
  const edge &e4 = p4.first;
  const edge &e5 = p5.first;

  boost::get(boost::edge_weight, g, e1) = 4;
  boost::get(boost::edge_weight, g, e2) = 1;
  boost::get(boost::edge_weight, g, e3) = 1;
  boost::get(boost::edge_weight, g, e4) = 1;
  boost::get(boost::edge_weight, g, e5) = 5;

  // Search for edge-disjoint paths of minimal weight.
  auto opaths = gd_search(0, 3, g);
  assert(opaths);
  auto paths = opaths.value();

  // The size of the first path should be two edges.
  BOOST_CHECK(paths.first.size() == 2);
  // The size of the second path should be two edges.
  BOOST_CHECK(paths.second.size() == 2);

  // The first path should have edges e1, and e2.
  {
    auto i = paths.first.begin();
    BOOST_CHECK(*i == e1);
    ++i;
    BOOST_CHECK(*i == e2);
    ++i;
    BOOST_CHECK(i == paths.first.end());
  }

  // The second path should have edges e4, and e5.
  {
    auto i = paths.second.begin();
    BOOST_CHECK(*i == e4);
    ++i;
    BOOST_CHECK(*i == e5);
    ++i;
    BOOST_CHECK(i == paths.second.end());
  }
}

// -------------------------------------------------------------------
//
//     e1      e2
//    --1--   --2--
//   /     \ /     \
// (0)     (1)     (2)
//   \     / \     /
//    --3--   --4--
//     e3      e4
//
BOOST_AUTO_TEST_CASE(ideas_2)
{
  graph g(3);
  auto p1 = boost::add_edge(0, 1, g);
  auto p2 = boost::add_edge(1, 2, g);
  auto p3 = boost::add_edge(0, 1, g);
  auto p4 = boost::add_edge(1, 2, g);

  assert(p1.second);
  assert(p2.second);
  assert(p3.second);
  assert(p4.second);

  const edge &e1 = p1.first;
  const edge &e2 = p2.first;
  const edge &e3 = p3.first;
  const edge &e4 = p4.first;

  boost::get(boost::edge_weight, g, e1) = 1;
  boost::get(boost::edge_weight, g, e2) = 2;
  boost::get(boost::edge_weight, g, e3) = 3;
  boost::get(boost::edge_weight, g, e4) = 4;

  // Search for edge-disjoint paths of minimal weight.
  auto opaths = gd_search(0, 2, g);
  assert(opaths);
  auto paths = opaths.value();

  // The size of the first path should be two edges.
  BOOST_CHECK(paths.first.size() == 2);
  // The size of the second path should be two edges.
  BOOST_CHECK(paths.second.size() == 2);

  // The first path should have edges e1, and e2.
  {
    auto i = paths.first.begin();
    BOOST_CHECK(*i == e1);
    ++i;
    BOOST_CHECK(*i == e2);
    ++i;
    BOOST_CHECK(i == paths.first.end());
  }

  // The second path should have edges e4, and e5.
  {
    auto i = paths.second.begin();
    BOOST_CHECK(*i == e3);
    ++i;
    BOOST_CHECK(*i == e4);
    ++i;
    BOOST_CHECK(i == paths.second.end());
  }
}
