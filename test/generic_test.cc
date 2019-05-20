#define BOOST_TEST_MODULE ideas

#include "adaptive_units.hpp"
#include "gd.hpp"
#include "graph.hpp"
#include "units.hpp"
#include "utils.hpp"

#include <boost/test/unit_test.hpp>

#include <optional>
#include <queue>
#include <utility>
#include <tuple>

using namespace std;

// -------------------------------------------------------------------
//
//     e1      e2
//    --1--   --2--
//   /     \ /     \
// (0)     (1)     (2)
//   \     / \     /
//    --1--   --1--
//     e3      e4
//
BOOST_AUTO_TEST_CASE(ideas_1)
{
  // Make sure we've got the modulation model right.
  using au = adaptive_units<COST>;
  // Set the maximal reach of the spectrally-worse (m = 1) modulation.
  au::set_reach_1(8);
  assert(au::units(1, 0) == 1);
  assert(au::units(1, 1) == 1);
  assert(au::units(1, 2) == 2);
  assert(au::units(1, 3) == 3);
  assert(au::units(1, 4) == 3);
  assert(au::units(1, 5) == 4);
  assert(au::units(1, 6) == 4);
  assert(au::units(1, 7) == 4);
  assert(au::units(1, 8) == 4);
  assert(au::units(1, 9) == std::numeric_limits<int>::max());

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
  boost::get(boost::edge_weight, g, e3) = 1;
  boost::get(boost::edge_weight, g, e4) = 1;

  // The SU.
  boost::get(boost::edge_su, g, e1) = SU{CU{0, 2}};
  boost::get(boost::edge_su, g, e2) = SU{CU{1, 4}};
  boost::get(boost::edge_su, g, e3) = SU{CU{0, 4}};
  boost::get(boost::edge_su, g, e4) = SU{CU{0, 4}};

  // Search for edge-disjoint paths of minimal weight.
  demand d(npair(0, 2), 1);
  auto opaths = get<4>(gd(g, d, CU{0, 4}));
  assert(opaths);
  auto paths = opaths.value();

  // The first path should have two edges.
  BOOST_CHECK(paths.first.second.size() == 2);
  // The length of the first path should be 2.
  BOOST_CHECK(get_path_length(g, paths.first.second) == 2);
  // The CU of the first path should be CU(0, 2);
  BOOST_CHECK(paths.first.first == CU(0, 2));

  // The first path should have edges e1, and e4.
  {
    auto i = paths.first.second.begin();
    BOOST_CHECK(*i == e1);
    ++i;
    BOOST_CHECK(*i == e4);
    ++i;
    BOOST_CHECK(i == paths.first.second.end());
  }

  // The second path should have two edges.
  BOOST_CHECK(paths.second.second.size() == 2);
  // The length of the second path should be 3.
  BOOST_CHECK(get_path_length(g, paths.second.second) == 3);
  // The CU of the second path should be CU(1, 4);
  BOOST_CHECK(paths.second.first == CU(1, 4));

  // The second path should have edges e3, and e2.
  {
    auto i = paths.second.second.begin();
    BOOST_CHECK(*i == e3);
    ++i;
    BOOST_CHECK(*i == e2);
    ++i;
    BOOST_CHECK(i == paths.second.second.end());
  }
}

// -------------------------------------------------------------------
//
//     e1      e2
//    --1--   --1--
//   /     \ /     \
// (0)     (1)     (2)
//   \     / \     /
//    --2--   --1--
//     e3      e4
//
BOOST_AUTO_TEST_CASE(ideas_1a)
{
  // Make sure we've got the modulation model right.
  using au = adaptive_units<COST>;
  // Set the maximal reach of the spectrally-worse (m = 1) modulation.
  au::set_reach_1(8);
  assert(au::units(1, 0) == 1);
  assert(au::units(1, 1) == 1);
  assert(au::units(1, 2) == 2);
  assert(au::units(1, 3) == 3);
  assert(au::units(1, 4) == 3);
  assert(au::units(1, 5) == 4);
  assert(au::units(1, 6) == 4);
  assert(au::units(1, 7) == 4);
  assert(au::units(1, 8) == 4);
  assert(au::units(1, 9) == std::numeric_limits<int>::max());

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
  boost::get(boost::edge_weight, g, e2) = 1;
  boost::get(boost::edge_weight, g, e3) = 2;
  boost::get(boost::edge_weight, g, e4) = 1;

  // The SU.
  boost::get(boost::edge_su, g, e1) = SU{CU{0, 3}};
  boost::get(boost::edge_su, g, e2) = SU{CU{0, 2}};
  boost::get(boost::edge_su, g, e3) = SU{CU{0, 3}};
  boost::get(boost::edge_su, g, e4) = SU{CU{0, 3}};

  // Search for edge-disjoint paths of minimal weight.
  demand d(npair(0, 2), 1);
  auto opaths = get<4>(gd(g, d, CU{0, 4}));
  assert(opaths);
  auto paths = opaths.value();

  // The first path should have two edges.
  BOOST_CHECK(paths.first.second.size() == 2);
  // The length of the first path should be 2.
  BOOST_CHECK(get_path_length(g, paths.first.second) == 2);
  // The CU of the first path should be CU(0, 2);
  BOOST_CHECK(paths.first.first == CU(0, 2));

  // The first path should have edges e1, and e4.
  {
    auto i = paths.first.second.begin();
    BOOST_CHECK(*i == e1);
    ++i;
    BOOST_CHECK(*i == e2);
    ++i;
    BOOST_CHECK(i == paths.first.second.end());
  }

  // The second path should have two edges.
  BOOST_CHECK(paths.second.second.size() == 2);
  // The length of the second path should be 3.
  BOOST_CHECK(get_path_length(g, paths.second.second) == 3);
  // The CU of the second path should be CU(0, 3);
  BOOST_CHECK(paths.second.first == CU(0, 3));

  // The second path should have edges e3, and e4.
  {
    auto i = paths.second.second.begin();
    BOOST_CHECK(*i == e3);
    ++i;
    BOOST_CHECK(*i == e4);
    ++i;
    BOOST_CHECK(i == paths.second.second.end());
  }
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
BOOST_AUTO_TEST_CASE(ideas_2)
{
  // Make sure we've got the modulation model right.
  using au = adaptive_units<COST>;
  // Set the maximal reach of the spectrally-worse (m = 1) modulation.
  au::set_reach_1(8);
  assert(au::units(1, 0) == 1);
  assert(au::units(1, 1) == 1);
  assert(au::units(1, 2) == 2);
  assert(au::units(1, 3) == 3);
  assert(au::units(1, 4) == 3);
  assert(au::units(1, 5) == 4);
  assert(au::units(1, 6) == 4);
  assert(au::units(1, 7) == 4);
  assert(au::units(1, 8) == 4);
  assert(au::units(1, 9) == std::numeric_limits<int>::max());

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

  // The lengths.
  boost::get(boost::edge_weight, g, e1) = 3;
  boost::get(boost::edge_weight, g, e2) = 1;
  boost::get(boost::edge_weight, g, e3) = 1;
  boost::get(boost::edge_weight, g, e4) = 1;
  boost::get(boost::edge_weight, g, e5) = 4;

  // The SU.
  boost::get(boost::edge_su, g, e1) = SU{CU{0, 4}};
  boost::get(boost::edge_su, g, e2) = SU{CU{0, 4}};
  boost::get(boost::edge_su, g, e3) = SU{CU{0, 4}};
  boost::get(boost::edge_su, g, e4) = SU{CU{0, 4}};
  boost::get(boost::edge_su, g, e5) = SU{CU{0, 4}};

  // Search for edge-disjoint paths of minimal weight.
  demand d(npair(0, 3), 1);
  auto opaths = get<4>(gd(g, d, CU{0, 4}));
  assert(opaths);
  auto paths = opaths.value();

  // The size of the first path should be two edges.
  BOOST_CHECK(paths.first.second.size() == 2);
  // The CU of the first path should be CU(0, 3);
  BOOST_CHECK(paths.first.first == CU(0, 3));
  // The size of the second path should be two edges.
  BOOST_CHECK(paths.second.second.size() == 2);
  // The CU of the second path should be CU(0, 4);
  BOOST_CHECK(paths.first.first == CU(0, 3));

  // The first path should have edges e1, and e2.
  {
    auto i = paths.first.second.begin();
    BOOST_CHECK(*i == e1);
    ++i;
    BOOST_CHECK(*i == e2);
    ++i;
    BOOST_CHECK(i == paths.first.second.end());
  }

  // The second path should have edges e4, and e5.
  {
    auto i = paths.second.second.begin();
    BOOST_CHECK(*i == e4);
    ++i;
    BOOST_CHECK(*i == e5);
    ++i;
    BOOST_CHECK(i == paths.second.second.end());
  }
}

// -------------------------------------------------------------------
//
// This test is similar to the one above, but we add parallel edges.
// We add e1a, which is shortest than e2, but e1a doesn't have enough
// spectrum.  We add e2a, so that the primary path goes through e2a,
// and edge e5 is not used.
//
//      -------(1)-------
//     /       /|\       \
// e1a 5   e1 4 | 1 e2   1 e2a
//     \     /  |  \     /
//      \   /   |   \   /
//       \ /    |    \ /
//       (0)    1 e3 (3)
//         \    |    /
//       e4 1   |   5 e5
//           \  |  /
//            \ | /
//             \|/
//             (2)
//
BOOST_AUTO_TEST_CASE(ideas_3)
{
  // Make sure we've got the modulation model right.
  using au = adaptive_units<COST>;
  // Set the maximal reach of the spectrally-worse (m = 1) modulation.
  au::set_reach_1(8);
  assert(au::units(1, 0) == 1);
  assert(au::units(1, 1) == 1);
  assert(au::units(1, 2) == 2);
  assert(au::units(1, 3) == 3);
  assert(au::units(1, 4) == 3);
  assert(au::units(1, 5) == 4);
  assert(au::units(1, 6) == 4);
  assert(au::units(1, 7) == 4);
  assert(au::units(1, 8) == 4);
  assert(au::units(1, 9) == std::numeric_limits<int>::max());

  graph g(4);
  auto p1 = boost::add_edge(0, 1, g);
  auto p1a = boost::add_edge(0, 1, g);
  auto p2 = boost::add_edge(1, 3, g);
  auto p2a = boost::add_edge(1, 3, g);
  auto p3 = boost::add_edge(1, 2, g);
  auto p4 = boost::add_edge(0, 2, g);
  auto p5 = boost::add_edge(2, 3, g);

  assert(p1.second);
  assert(p1a.second);
  assert(p2.second);
  assert(p2a.second);
  assert(p3.second);
  assert(p4.second);
  assert(p5.second);

  const edge &e1 = p1.first;
  const edge &e1a = p1a.first;
  const edge &e2 = p2.first;
  const edge &e2a = p2a.first;
  const edge &e3 = p3.first;
  const edge &e4 = p4.first;
  const edge &e5 = p5.first;

  // The lengths.
  boost::get(boost::edge_weight, g, e1) = 4;
  boost::get(boost::edge_weight, g, e1a) = 5;
  boost::get(boost::edge_weight, g, e2) = 1;
  boost::get(boost::edge_weight, g, e2a) = 1;
  boost::get(boost::edge_weight, g, e3) = 1;
  boost::get(boost::edge_weight, g, e4) = 1;
  boost::get(boost::edge_weight, g, e5) = 5;

  // The SU.
  boost::get(boost::edge_su, g, e1) = SU{CU{0, 4}};
  boost::get(boost::edge_su, g, e1a) = SU{CU{0, 4}};
  boost::get(boost::edge_su, g, e2) = SU{CU{0, 4}};
  boost::get(boost::edge_su, g, e2a) = SU{CU{1, 4}};
  boost::get(boost::edge_su, g, e3) = SU{CU{0, 4}};
  boost::get(boost::edge_su, g, e4) = SU{CU{0, 4}};
  boost::get(boost::edge_su, g, e5) = SU{CU{0, 4}};

  // Search for edge-disjoint paths of minimal weight.
  demand d(npair(0, 3), 1);
  auto opaths = get<4>(gd(g, d, CU{0, 4}));
  assert(opaths);
  auto paths = opaths.value();

  // The first path should have three edges.
  BOOST_CHECK(paths.first.second.size() == 3);
  // The length of the first path should be 3.
  BOOST_CHECK(get_path_length(g, paths.first.second) == 3);
  // The CU of the first path should be CU(1, 4);
  BOOST_CHECK(paths.first.first == CU(1, 4));

  // The first path should have edges e4, e3, and e2a.
  {
    auto i = paths.first.second.begin();
    BOOST_CHECK(*i == e4);
    ++i;
    BOOST_CHECK(*i == e3);
    ++i;
    BOOST_CHECK(*i == e2a);
    ++i;
    BOOST_CHECK(i == paths.first.second.end());
  }

  // The second path should have two edges.
  BOOST_CHECK(paths.second.second.size() == 2);
  // The length of the second path should be 5.
  BOOST_CHECK(get_path_length(g, paths.second.second) == 5);
  // The CU of the second path should be CU(0, 4);
  BOOST_CHECK(paths.second.first == CU(0, 4));

  // The second path should have edges e1, and e2.
  {
    auto i = paths.second.second.begin();
    BOOST_CHECK(*i == e1);
    ++i;
    BOOST_CHECK(*i == e2);
    ++i;
    BOOST_CHECK(i == paths.second.second.end());
  }
}
