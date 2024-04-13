std::unordered_map<std::string, Node*> Graph::BuildNodeNameIndex() const {
  std::unordered_map<std::string, Node*> result;
  for (Node* n : nodes()) {
    result[n->name()] = n;
  }
  return result;
}