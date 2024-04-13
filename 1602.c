void Node::Initialize(int id, int cost_id,
                      std::shared_ptr<NodeProperties> props,
                      Node::NodeClass node_class) {
  DCHECK_EQ(id_, -1);
  DCHECK(in_edges_.empty());
  DCHECK(out_edges_.empty());
  id_ = id;
  cost_id_ = cost_id;

  props_ = std::move(props);
  class_ = node_class;
}