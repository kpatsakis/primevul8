std::string Edge::DebugString() const {
  return strings::Printf("[id=%d %s:%d -> %s:%d]", id_, src_->name().c_str(),
                         src_output_, dst_->name().c_str(), dst_input_);
}