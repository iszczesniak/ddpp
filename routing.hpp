#ifndef ROUTING_HPP
#define ROUTING_HPP

#include "graph.hpp"
#include "sim.hpp"

#include <optional>

class routing: public sim
{  
public:
  // The routing algorithm:
  // gd - generic Dijkstra
  // bf - brute force
  // ee - edge exclusion
  enum class rt_t {gd, bf, ee};

  // Return the string of the routing type.
  static std::string
  to_string(routing::rt_t rt);

  // Try to set up the demand, i.e., find the path, and allocate
  // resources.  The result returned is the supath set up.
  static std::optional<cupp>
  set_up(graph &g, const demand &d);

  // Try to set up the demand with the given SU, i.e., find the path,
  // and allocate resources.  The result returned is the cupath set
  // up.
  static std::optional<cupp>
  set_up(graph &g, const demand &d, const CU &cu);

  // Search for a path using a given algorithm.  If function fails, no
  // result is returned.
  static std::optional<cupp>
  search(graph &g, const demand &d, const CU &cu, rt_t rt);

  // Tear down the cupath in the graph.  This process puts back the
  // units on the edges that are used by the path.
  static void
  tear_down(graph &g, const cupath &p);

  // Tear down the cupp in the graph.  This process puts back the
  // units on the edges that are used by the path pair.
  static void
  tear_down(graph &g, const cupp &p);

  // What another routing algorithms to run.
  static void add_another_algorithm(const rt_t rt);

protected:
  // Set up the given cupath.  This process takes the units on the
  // edges that are used by the path.  The function always succeeds,
  // otherwise an assertion fails.
  static void
  set_up(graph &g, const cupath &p);

  // Set up the given cupp.  This process takes the units on the edges
  // that are used by the path pair.  The function always succeeds,
  // otherwise an assertion fails.
  static void
  set_up(graph &g, const cupp &p);

  // What another routing algorithms to use.
  static std::set<rt_t> m_aras;
};

#endif /* ROUTING_HPP */
