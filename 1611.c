Status Graph::IsValidNode(const Node* node) const {
  if (node == nullptr) {
    return errors::InvalidArgument("Node is null");
  }
  const int id = node->id();
  if (id < 0) {
    return errors::InvalidArgument("node id ", id, " is less than zero");
  }
  if (static_cast<size_t>(id) >= nodes_.size()) {
    return errors::InvalidArgument(
        "node id ", id, " is >= than number of nodes in graph ", nodes_.size());
  }
  if (nodes_[id] != node) {
    return errors::InvalidArgument("Node with id ", id,
                                   " is different from the passed in node. "
                                   "Does it belong to a different graph?");
  }
  return Status::OK();
}