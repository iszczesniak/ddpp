#include "connection.hpp"
#include "routing.hpp"
#include "utils.hpp"

#include <algorithm>
#include <iostream>
#include <optional>
#include <utility>

using namespace std;

int connection::counter = 0;

connection::connection(graph &g): m_g(g), m_id(counter++)
{
}

connection::~connection()
{
  if (is_established())
    tear_down();
}

const demand &
connection::get_demand() const
{
  return m_d;
}

bool
connection::is_established() const
{
  return m_p.has_value();
}

COST
connection::get_cost() const
{
  assert(is_established());
  return ::get_cost(m_g, m_p.value());
}

bool
connection::establish(const demand &d)
{
  // Make sure the connection is not established.
  assert(!is_established());

  // Remember the demand.
  m_d = d;

  // Set up the demand.
  m_p = routing::set_up(m_g, d);

  return is_established();
}

void
connection::tear_down()
{
  assert(is_established());
  routing::tear_down(m_g, m_p.value());
  m_p.reset();
}
