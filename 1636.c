Status Graph::AddWhileContext(StringPiece frame_name,
                              std::vector<Node*> enter_nodes,
                              std::vector<Node*> exit_nodes,
                              OutputTensor cond_output,
                              std::vector<OutputTensor> body_inputs,
                              std::vector<OutputTensor> body_outputs,
                              WhileContext** result) {
  auto pair = while_ctxs_.insert(std::pair<std::string, WhileContext>(
      std::string(frame_name),
      WhileContext(frame_name, std::move(enter_nodes), std::move(exit_nodes),
                   cond_output, std::move(body_inputs),
                   std::move(body_outputs))));
  if (!pair.second) {
    *result = nullptr;
    return errors::InvalidArgument("WhileContext with frame name '", frame_name,
                                   "' already exists");
  }
  *result = &pair.first->second;
  return Status::OK();
}