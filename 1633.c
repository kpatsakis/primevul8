void Node::set_name(std::string name) {
  MaybeCopyOnWrite();
  props_->node_def.set_name(std::move(name));
}