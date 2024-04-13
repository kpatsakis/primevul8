Status Graph::IsValidInputTensor(const Node* node, int idx) const {
  TF_RETURN_IF_ERROR(IsValidNode(node));
  if (idx >= node->num_inputs() || idx < 0) {
    return errors::OutOfRange("Node '", node->name(), "' (type: '",
                              node->op_def().name(),
                              "', num of inputs: ", node->num_inputs(),
                              ") does not have ", "input ", idx);
  }
  return Status::OK();
}