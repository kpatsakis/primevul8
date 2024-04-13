const protobuf::RepeatedPtrField<std::string>& Node::requested_inputs() const {
  return def().input();
}