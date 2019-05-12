#ifndef GRAPH_INT_HPP
#define GRAPH_INT_HPP

#include "units.hpp"

template <typename Graph>
using Vertex = typename Graph::vertex_descriptor;

template <typename Graph>
using Edge = typename Graph::edge_descriptor;

template <typename Graph>
using Path = std::list<Edge<Graph>>;

template <typename Graph>
using CU_Path = std::pair<CU, Path<Graph>>;

#endif /* GRAPH_INT_HPP */
