void Node::UpdateProperties() {
  DataTypeVector inputs;
  DataTypeVector outputs;
  Status status =
      InOutTypesForNode(props_->node_def, *(props_->op_def), &inputs, &outputs);
  if (!status.ok()) {
    LOG(ERROR) << "Failed at updating node: " << status;
    return;
  }
  if (props_->input_types != inputs || props_->output_types != outputs) {
    if (TF_PREDICT_TRUE(props_.use_count() == 1)) {
      props_->input_types = inputs;
      props_->input_types_slice = props_->input_types;
      props_->output_types = outputs;
      props_->output_types_slice = props_->output_types;
    } else {
      props_ = std::make_shared<NodeProperties>(
          props_->op_def, std::move(props_->node_def), inputs, outputs);
    }
  }
}