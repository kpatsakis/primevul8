bool OutputTensor::operator==(const OutputTensor& other) const {
  return node == other.node && index == other.index;
}