uint64 InputTensor::Hash::operator()(InputTensor const& s) const {
  return Hash64Combine(std::hash<const Node*>()(s.node),
                       std::hash<int>()(s.index));
}