void Node::ClearTypeInfo() {
  if (props_->node_def.has_experimental_type()) {
    MaybeCopyOnWrite();
    props_->node_def.clear_experimental_type();
  }
}