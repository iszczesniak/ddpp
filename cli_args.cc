#include "cli_args.hpp"

#include "connection.hpp"
#include "routing.hpp"
#include "utils.hpp"

#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include <boost/program_options.hpp>

// Option strings.
#define K_S "K"
#define MLC_S "mlc"
#define NET_S "net"
#define ST_S "st"
#define POPULATION_S "population"
#define BF_S "bf"

using namespace std;
namespace po = boost::program_options;

cli_args
process_cli_args(int argc, const char *argv[])
{
  cli_args result;

  try
    {
      // General options.
      po::options_description gen("General options");
      gen.add_options()
        ("help,h", "produce help message");

      // Network options.
      po::options_description net("Network options");
      net.add_options()
        (NET_S, po::value<string>()->required(),
         "the network file name")

        ("units", po::value<int>()->required(),
         "the number of units")

        (BF_S, "run the brtforce search");

      // Traffic options.
      po::options_description tra("Traffic options");
      tra.add_options()
        ("ol", po::value<double>()->required(),
         "the offered load")

        ("mht", po::value<double>()->required(),
         "the mean holding time")

        ("mnu", po::value<double>()->required(),
         "the mean number of units");

      // Simulation options.
      po::options_description sim("Simulation options");
      sim.add_options()
        ("seed", po::value<int>()->default_value(1),
         "the seed of the random number generator")

        (POPULATION_S, po::value<string>()->required(),
         "the population name");

      po::options_description all("Allowed options");
      all.add(gen).add(net).add(tra).add(sim);
      
      po::variables_map vm;
      po::store(po::command_line_parser(argc, argv).options(all).run(), vm);

      if (vm.count("help"))
        {
          cout << all << "\n";
          exit(0);
        }

      // If there is something wrong with parameters, we will get
      // notified automatically and the program will exit.
      po::notify(vm);

      // The network options.
      result.net = vm[NET_S].as<string>();

      result.units = vm["units"].as<int>();

      if (vm.count(BF_S))
        result.bf = true;

      // The traffic options.
      result.ol = vm["ol"].as<double>();
      result.mht = vm["mht"].as<double>();
      result.mnu = vm["mnu"].as<double>();

      // The simulation options.
      result.seed = vm["seed"].as<int>();
      result.population = vm[POPULATION_S].as<string>();
    }
  catch(const std::exception& e)
    {
      cerr << e.what() << std::endl;
      exit(1);
    }

  return result;
}
