#include "adaptive_units.hpp"
#include "cli_args.hpp"
#include "graph.hpp"
#include "sim.hpp"
#include "stats.hpp"
#include "utils.hpp"

using namespace std;

int
simulate(const cli_args &args_para)
{
  cli_args args = args_para;

  if (args.bf)
    routing::add_another_algorithm(routing::rt_t::bf);

  // Initialize the random number engine of the simulation.
  sim::rne().seed(args.seed);

  // The graph.
  graph &g = sim::mdl();

  // Load the graph.
  if (!load_graphviz(args.net, g))
    return 1;

  set_units(g, args.units);

  // Make sure there is only one component.
  assert(is_connected(g));

  dbl_acc hop_acc;
  dbl_acc len_acc;
  calc_sp_stats(g, hop_acc, len_acc);

  // Report the length of the longest shortest path.
  adaptive_units<COST>::set_reach_1(ba::max(len_acc) * 1.5);

  // Calculate the mean connection arrival time.
  args.mcat = calc_mcat(g, ba::mean(hop_acc), args.mht,
                        args.mnu, args.ol);

  // The stats kickoff time.
  args.kickoff = 5 * args.mht;

  // The simulation time limit.
  args.sim_time = 15 * args.mht;

  // The traffic module.
  traffic t(args.mcat, args.mht, args.mnu);

  // The stats module.
  stats s(args, t);

  // Run the simulation.
  sim::run(args.sim_time);

  return 0;
}

int
main(int argc, const char* argv[])
{
  cli_args args = process_cli_args(argc, argv);
  return simulate(args);
}
