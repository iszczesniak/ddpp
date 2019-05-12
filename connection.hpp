#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "graph.hpp"

#include <optional>
#include <utility>

// The type of the connection.  It can establish, reconfigure and tear
// down a connection, but it doesn't report the statistics.
class connection
{
public:
  connection(graph &g);
  ~connection();

  const demand &
  get_demand() const;

  bool
  is_established() const;

  // Establish the connection for the given demand.  True if
  // successful.  If unsuccessful, the state of the object doesn't
  // change.
  bool
  establish(const demand &d);

  // Return the cost of the protected connection.  The cost is the sum
  // of the costs of both paths.  A cost of a path is the product of
  // length and the number of units.
  COST
  get_cost() const;

  void
  tear_down();

private:
  graph &m_g;
  demand m_d;
  std::optional<cupp> m_p;

  int m_id;

  static int counter;
};

#endif /* CONNECTION_HPP */
