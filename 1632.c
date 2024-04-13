Status Node::input_node(int idx, const Node** const_n) const {
  Node* n;
  TF_RETURN_IF_ERROR(input_node(idx, &n));
  *const_n = n;
  return Status::OK();
}