#ifndef CLI_ARGS_HPP
#define CLI_ARGS_HPP

#include "connection.hpp"
#include "routing.hpp"

#include <optional>
#include <string>

/**
 * These are the program arguments.  In this single class we store all
 * information passed at the command line.
 */
struct cli_args
{
  /// -----------------------------------------------------------------
  /// The network and routing options
  /// -----------------------------------------------------------------
  
  /// The network file name.
  std::string net;

  /// The number of units.
  int units;

  // Use the generic Dijkstra search.
  bool gd = false;

  // Use the brute force search.
  bool bf = false;

  // Use the edge exclusion search.
  bool ee = false;

  /// -----------------------------------------------------------------
  /// The traffic options
  /// -----------------------------------------------------------------

  /// The mean client arrival time.
  double mcat;

  /// The offered load.
  double ol;

  /// The mean holding time.
  double mht;

  /// The mean number of units.
  double mnu;

  /// -----------------------------------------------------------------
  /// The simulation options
  /// -----------------------------------------------------------------

  /// The seed
  int seed;

  /// The population name.
  std::string population;

  /// The kickoff time for stats.
  double kickoff;

  /// The limit on the simulation time.
  double sim_time;
};

/**
 * This function parses the command-line arguments.
 */
cli_args
process_cli_args(int argc, const char* argv[]);

#endif /* CLI_ARGS_HPP */
