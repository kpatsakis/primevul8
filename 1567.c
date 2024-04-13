Status Node::input_tensor(int idx, OutputTensor* t) const {
  const Edge* e;
  TF_RETURN_IF_ERROR(input_edge(idx, &e));
  DCHECK(e != nullptr);
  *t = OutputTensor(e->src(), e->src_output());
  return Status::OK();
}