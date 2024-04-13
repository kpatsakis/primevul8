Node* Graph::AllocateNode(std::shared_ptr<NodeProperties> props,
                          const Node* cost_node, Node::NodeClass node_class) {
  Node* node = nullptr;
  if (free_nodes_.empty()) {
    node = new (arena_.Alloc(sizeof(Node))) Node;  // placement new
  } else {
    node = free_nodes_.back();
    free_nodes_.pop_back();
  }
  node->graph_ = this;
  const int id = nodes_.size();
  int cost_id = cost_node ? cost_node->cost_id() : id;
  node->Initialize(id, cost_id, std::move(props), node_class);
  nodes_.push_back(node);
  ++num_nodes_;
  return node;
}