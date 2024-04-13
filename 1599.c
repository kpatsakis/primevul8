void Node::MaybeCopyOnWrite() {
  // TODO(mdan): As nodes become more dynamic, this may not be worth the cost.
  // NodeProperties may be shared between Nodes. Make a copy if so.
  if (!props_.unique()) {
    props_ = std::make_shared<NodeProperties>(*props_);
  }
}