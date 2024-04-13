Status Node::input_edges(std::vector<const Edge*>* input_edges) const {
  input_edges->clear();
  input_edges->resize(num_inputs(), nullptr);

  for (const Edge* edge : in_edges()) {
    if (edge->IsControlEdge()) continue;
    if (edge->dst_input() < 0 || edge->dst_input() >= num_inputs()) {
      return errors::Internal("Invalid edge input number ", edge->dst_input());
    }
    if ((*input_edges)[edge->dst_input()] != nullptr) {
      return errors::Internal("Duplicate edge input number: ",
                              edge->dst_input());
    }
    (*input_edges)[edge->dst_input()] = edge;
  }

  for (int i = 0; i < num_inputs(); ++i) {
    if ((*input_edges)[i] == nullptr) {
      return errors::InvalidArgument("Missing edge input number: ", i);
    }
  }
  return Status::OK();
}