#include "shared.hpp"

#include "adaptive_units.hpp"
#include "graph.hpp"
#include "utils.hpp"

using namespace std;

std::optional<std::pair<cupath, cupath> >
trace(const graph &g, const tn_t &tn, const unsigned &ncu)
{
  std::optional<std::pair<cupath, cupath> > result;
  
  optional<cupath> p1, p2;
  optional<COST> c1, c2;

  // Trace back the paths.  We start at the current tree node, i.e.,
  // tnp, and end at the root (which has nullptr as the parent node).
  // In every iteration we put an edge into either p1 or p2.
  for(const tn_t *ptr = &tn; std::get<4>(*ptr).get() != nullptr;
      ptr = std::get<4>(*ptr).get())
    {
      // Here we're processing the tree node tn.
      const tn_t tn = *ptr;
      // The edge of tn.  We need to put it into p1 or p2.
      const edge &e = std::get<2>(tn);
      // This is the new vertex in the vertex pair.
      const vertex &t = boost::target(e, g);

      // If true, the first label is new.
      bool first = std::get<3>(tn);

      // The vertex pair of the tn.
      const vertex_pair_t tnvp = std::get<0>(tn);
      // Let's make sure that the target of the edge is the right
      // vertex (either first or second) in the vertex pair of the
      // tree node.
      assert((first ? tnvp.first : tnvp.second) == t);

      // The label pair of the tree node we're processing.
      const label_pair_t &l = std::get<1>(tn);
      // This is the 1st label of tn.
      const label &tnl1 = l.first;
      // This is the 2nd label of tn.
      const label &tnl2 = l.second;
      // This is the cost and CU of the new label.
      const auto &[nc, nl_cu] = first ? tnl1 : tnl2;

      if (p1)
        {
          // The previous edge of path p1.
          const auto &pe = p1.value().second.front();
          // The cost of the previous edge of path p1.
          const auto &pec = boost::get(boost::edge_weight, g, pe);
          // The cu of path p1.
          const auto &pcu = p1.value().first;

          if (t == source(pe, g) && nl_cu.includes(pcu) &&
              nc + pec == c1.value())
            {
              bool yeah = true;

              // But that's OK.  If could happen that an edge could
              // fit to both paths p1, and p2.  For instance (that's a
              // real case I've got), we've got edge e = (19, 15), and
              // the label from which e originates: l1 = (463, {92,
              // 100}).  We need to decide to which path we should add
              // edge e:
              //
              // p1 = ({96, 100}, ((15, 17), (17, 20))
              //
              // p2 = ({92, 96}, ((15, 20)))
              //
              // For path p1, we remember that node 17 was reached at
              // cost 532 (c1.value()), and know that edge (15, 17) is
              // of cost 69, so the label from which edge e originates
              // should have the cost of 463.  The CU of l1 should
              // include the CU of p1, and that's true.  So we might
              // think that e should go to p1.
              //
              // However, e should go to p2.  For path p2, we remember
              // that node 20 was reached at cost 478, and know that
              // edge (15, 20) is of cost 15, so the label from which
              // edge e originates should have the cost of 463.  The
              // CU of l1 should include the CU of p2, and that's true
              // too.
              //
              // The key here is to consider the other path label l2 =
              // (463, {96, 100}), whose costs and CU should agree
              // with the other path, which is p1 here.  If not, the
              // edge should be put to p2.
              if (p2)
                {
                  const auto &[oc, ol_cu] = (first ? tnl2 : tnl1);
                  // The previous edge of path p2.
                  const auto &pe2 = p2.value().second.front();
                  // The cost of the previous edge of path p2.
                  const auto &pec2 = boost::get(boost::edge_weight,
                                                g, pe2);
                  // The cu of path p2.
                  const auto &pcu2 = p2.value().first;

                  if (!ol_cu.includes(pcu2) || oc + pec2 != c2.value())
                    yeah = false;
                }

              if (yeah)
                {
                  p1.value().second.push_front(e);
                  c1 = nc;
                  continue;
                }
            }
        }

      if (p2)
        {
          // The previous edge of path p2.
          const auto &pe = p2.value().second.front();
          // The cost of the previous edge of path p2.
          const auto &pec = boost::get(boost::edge_weight, g, pe);
          // The cu of path p2.
          const auto &pcu = p2.value().first;

          if (t == source(pe, g) && nl_cu.includes(pcu) &&
              nc + pec == c2.value())
            {
              p2.value().second.push_front(e);
              c2 = nc;
              continue;
            }
        }

      // We get here only when there is no p1 or p2.
      assert(!p1 || !p2);

      // Choose the optional path.
      optional<cupath> &op = p1 ? p2 : p1;
      // And optional cost.
      optional<COST> &oc = p1 ? c2 : c1;
      // Make sure we chose the optional with no value.
      assert(op == nullopt);

      int units = adaptive_units<COST>::units(ncu, nc);
      // First-fit spectrum allocation policy.
      op.emplace(CU(nl_cu.min(), nl_cu.min() + units), path{e});
      oc = nc;
    }

  if (get_cost(g, p1.value()) <= get_cost(g, p2.value()))
    result = pair(p1.value(), p2.value());
  else
    result = pair(p2.value(), p1.value());

  return result;
}
