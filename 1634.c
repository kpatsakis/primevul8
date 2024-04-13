Status Graph::IsValidOutputTensor(const Node* node, int idx) const {
  TF_RETURN_IF_ERROR(IsValidNode(node));
  if (idx >= node->num_outputs() || idx < 0) {
    return errors::OutOfRange("Node '", node->name(), "' (type: '",
                              node->op_def().name(),
                              "', num of outputs: ", node->num_outputs(),
                              ") does not have ", "output ", idx);
  }
  return Status::OK();
}