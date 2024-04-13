Status Node::input_node(int idx, Node** n) const {
  const Edge* e;
  TF_RETURN_IF_ERROR(input_edge(idx, &e));
  if (e == nullptr) {
    *n = nullptr;
  } else {
    *n = e->src();
  }
  return Status::OK();
}