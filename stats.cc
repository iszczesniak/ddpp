#include "client.hpp"
#include "routing.hpp"
#include "stats.hpp"
#include "utils.hpp"

#include <boost/range.hpp>

#include <cmath>
#include <iostream>
#include <sstream>

using namespace std;

// The singleton of the class.  The compiler initializes it to null.
stats *stats::singleton;

stats::stats(const cli_args &args, const traffic &tra):
  m_args(args), m_tra(tra),
  m_dt((args.sim_time - args.kickoff) / nom)
{
  assert(!singleton);
  singleton = this;

  // We start collecting the utilization stats at the kickoff time.
  schedule(args.kickoff);
}

template <typename T>
void
output(const string &txt, const T &v)
{
  cout << txt << " " << v << endl;
}

stats::~stats()
{
  // The population name.
  output("population", m_args.population);

  // The network utilization.
  output("utilization", ba::mean(m_utilization));

  // The probability of establishing a connection.
  output("pec", ba::mean(m_pec));
  // The bitrate blocking probability.
  output("bbp", ba::sum(m_bb) / static_cast<double>(ba::sum(m_rb)));
  // The mean cost of an established connection.
  output("cstec", ba::mean(m_cstec));

  // The algorithm statistics.
  for (const auto &e: m_mmwus)
    {
      // The routing type.
      auto rt = e.first;
      const string prefix = routing::to_string(rt) + '_';
      output(prefix + "mean_mmwu", ba::mean(m_mmwus[rt]));
      output(prefix + "max_mmwu", ba::max(m_mmwus[rt]));
      output(prefix + "mean_mpqc", ba::mean(m_mpqcs[rt]));
      output(prefix + "max_mpqc", ba::max(m_mpqcs[rt]));
      output(prefix + "mean_msc", ba::mean(m_mscs[rt]));
      output(prefix + "max_msc", ba::max(m_mscs[rt]));
      output(prefix + "mean_mqc", ba::mean(m_mqcs[rt]));
      output(prefix + "max_mqc", ba::max(m_mqcs[rt]));
    }

  // The number of currently active connections.
  output("conns", ba::mean(m_conns));
  // The capacity served.
  output("capser", ba::mean(m_capser));
  // The mean number of fragments on links.
  output("frags", ba::mean(m_frags));
  // The number of rejected connections.
  output("rejected", m_rejected);
}

stats &
stats::get()
{
  return *singleton;
}

void
stats::operator()(const double st)
{
  // The current network utilization.
  m_utilization(calculate_utilization(m_mdl));
  // The number of connections served.
  m_conns(m_tra.nr_clients());
  // The capacity served.
  m_capser(m_tra.capacity_served());
  // The number of fragments.
  m_frags(calculate_frags());

  schedule(st);
}

// Schedule the next event based on the current time 0.
void
stats::schedule(const double t)
{
  // We call the stats every second.
  module::schedule(t + m_dt);
}

void
stats::report(const connection &conn)
{
  if (m_args.kickoff <= now())
    {
      bool status = conn.is_established();
      m_pec(status);

      // The requested bitrate.
      auto C = conn.get_demand().second;

      // Record the requested bitrate.
      m_rb(C);

      if (status)
        {
          m_cstec(conn.get_cost());
        }
      else
        {
          // Record the blocked bitrate.
          m_bb(C);
          // Increment the number of rejected connections.
          ++m_rejected;
        }
    }
}

void
stats::algo_perf(const routing::rt_t rt, const double dt,
                 const int mmwu, const int mpqc,
                 const int msc, const int mqc)
{
  if (m_args.kickoff <= now())
    {
      cerr << routing::to_string(rt) << " " << dt << endl;
      m_mmwus[rt](mmwu);
      m_mpqcs[rt](mpqc);
      m_mscs[rt](msc);
      m_mqcs[rt](mqc);
    }
}

double
stats::calculate_frags()
{
  dbl_acc frags;

  // Iterate over all edges.
  graph::edge_iterator ei, ee;
  for (tie(ei, ee) = boost::edges(m_mdl); ei != ee; ++ei)
    {
      const edge e = *ei;
      const SU &su = boost::get(boost::edge_su, m_mdl, e);
      int f = su.size();
      frags(f);
    }

  return ba::mean(frags);
}