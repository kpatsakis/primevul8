uint64 OutputTensor::Hash::operator()(OutputTensor const& s) const {
  return Hash64Combine(std::hash<const Node*>()(s.node),
                       std::hash<int>()(s.index));
}