bool InputTensor::operator==(const InputTensor& other) const {
  return node == other.node && index == other.index;
}