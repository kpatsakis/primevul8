Status Node::input_edge(int idx, const Edge** e) const {
  if (idx < 0 || idx >= num_inputs()) {
    return errors::InvalidArgument("Invalid input_edge index: ", idx, ", Node ",
                                   name(), " only has ", num_inputs(),
                                   " inputs.");
  }

  // This does a linear search over the edges.  In the common case,
  // the number of elements is small enough that this search isn't
  // expensive.  Should it become a bottleneck, one can make an
  // optimization where, if the number of edges is small, we use
  // linear iteration, and if the number of edges is large, we perform
  // an indexing step during construction that keeps an array of Edges
  // indexed by pointer.  This would keep the size of each Node small
  // in the common case but make this function faster when the number
  // of edges is large.
  for (const Edge* edge : in_edges()) {
    if (edge->dst_input() == idx) {
      *e = edge;
      return Status::OK();
    }
  }

  return errors::NotFound("Could not find input edge ", idx, " for ", name());
}