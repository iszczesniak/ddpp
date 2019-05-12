#ifndef SHARED_HPP
#define SHARED_HPP

#include "label.hpp"

#include <memory>
#include <optional>
#include <queue>

// Tree node type declared.
struct tn_t;

// Tree node type defined.
struct tn_t: std::tuple<vertex_pair_t, label_pair_t,
                        edge, bool, std::shared_ptr<tn_t> >
{
  using base = std::tuple<vertex_pair_t, label_pair_t,
                          edge, bool, std::shared_ptr<tn_t> >;

  using base::base;
};

// The tnt shared_ptr.
using tnsp_t = std::shared_ptr<tn_t>;

std::optional<std::pair<cupath, cupath> >
trace(const graph &g, const tn_t &tn, const unsigned &);

#endif // SHARED_HPP
