#ifndef STATS_HPP
#define STATS_HPP

#include "cli_args.hpp"
#include "connection.hpp"
#include "event.hpp"
#include "graph.hpp"
#include "module.hpp"
#include "routing.hpp"
#include "sim.hpp"
#include "traffic.hpp"

#include <chrono>
#include <map>
#include <optional>
#include <vector>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>

namespace ba = boost::accumulators;

class client;

class stats: public module<sim>
{
  // The singleton of the class.
  static stats *singleton;

  // The traffic of the run.
  const traffic &m_tra;

  // The arguments of the run.
  const cli_args &m_args;

  // The number of instantaneous measurements of the network state.
  const int nom = 100;

  // The time difference for taking the instantaneous measurements.
  const sim::time_type m_dt;

  // The accumulator type with double values.
  typedef ba::accumulator_set<double, ba::features<ba::tag::count,
                                                   ba::tag::mean,
                                                   ba::tag::max>> dbl_acc;

  typedef ba::accumulator_set<long, ba::features<ba::tag::sum>> sum_acc;

  // The utilization.
  dbl_acc m_utilization;

  // The blocking probability.
  std::map<routing::rt_t, dbl_acc> m_bp;
  // The blocked bitrate.
  std::map<routing::rt_t, sum_acc> m_bb;
  // The requested bitrate.
  std::map<routing::rt_t, sum_acc> m_rb;

  // The cost (CU * length) of the established protected connection.
  std::map<routing::rt_t, dbl_acc> m_cstec;

  // The time taken by a search.
  std::map<routing::rt_t, dbl_acc> m_t;

  // The memory usage.
  std::map<routing::rt_t, dbl_acc> m_mmwus;
  std::map<routing::rt_t, dbl_acc> m_mpqcs;
  std::map<routing::rt_t, dbl_acc> m_mscs;
  std::map<routing::rt_t, dbl_acc> m_mqcs;

  // The number of connections served.
  dbl_acc m_conns;
  // The capacity served.
  dbl_acc m_capser;
  // The number of fragments.
  dbl_acc m_frags;

public:
  stats(const cli_args &, const traffic &);

  ~stats();

  static stats &
  get();

  void
  schedule(const double t);

  void
  operator()(const double t);

  // Report the algorithm performance.
  void
  algo_perf(const routing::rt_t rt, const double dt,
            const int &ncu,
            const std::pair<std::array<unsigned long, 4>,
                            std::optional<cupp>> &p);

private:
  // Calculate the average number of fragments on a link.
  double
  calculate_frags();
};

#endif
