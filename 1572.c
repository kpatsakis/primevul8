void Node::ClearAttr(const std::string& name) {
  MaybeCopyOnWrite();
  (*props_->node_def.mutable_attr()).erase(name);
}