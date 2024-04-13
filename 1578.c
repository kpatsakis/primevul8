void Node::set_requested_device(const std::string& device) {
  MaybeCopyOnWrite();
  props_->node_def.set_device(device);
}