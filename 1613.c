void Node::set_original_node_names(const std::vector<std::string>& names) {
  MaybeCopyOnWrite();
  props_->node_def.mutable_experimental_debug_info()
      ->clear_original_node_names();
  if (!names.empty()) {
    *props_->node_def.mutable_experimental_debug_info()
         ->mutable_original_node_names() = {names.begin(), names.end()};
  }
}