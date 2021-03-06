#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "graph_int.hpp"

#include "units.hpp"

#include <list>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <boost/graph/adjacency_list.hpp>

namespace boost {
  // Describes the set of available units on an edge.
  enum edge_su_t {edge_su};

  BOOST_INSTALL_PROPERTY(edge, su);

  // Describes the number of units on an edge.
  enum edge_nou_t {edge_nou};

  BOOST_INSTALL_PROPERTY(edge, nou);
}

// The type of cost of reaching a vertex.
typedef double COST;

/**
 * The type of the graph we use.  The edge_su_t property describes the
 * units available, and not already taken.
 */
typedef
boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
		      boost::property<boost::vertex_name_t,
                                      std::string>,
                      boost::property<boost::edge_weight_t, COST,
                      boost::property<boost::edge_nou_t, unsigned,
                      boost::property<boost::edge_su_t, SU> > > >
graph;

typedef graph::edge_descriptor edge;
typedef graph::vertex_descriptor vertex;

// The path.
typedef std::list<edge> path;

// The list of paths.
typedef std::list<path> plist;

// The CU path.
typedef std::pair<CU, path> cupath;

// The CU path pair.
typedef std::pair<cupath, cupath> cupp;

// Node pair.
typedef std::pair<vertex, vertex> npair;

// The demand type: npair and the number of contiguous units (ncu).
typedef std::pair<npair, int> demand;

template <typename Graph>
using Vertex = typename Graph::vertex_descriptor;

template <typename Graph>
using Edge = typename Graph::edge_descriptor;

#endif /* GRAPH_HPP */
