const Edge* FindEdge(const Node* dst, int index) {
  for (const Edge* e : dst->in_edges()) {
    if (e->dst_input() == index) return e;
  }
  return nullptr;
}