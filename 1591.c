void Graph::RecycleEdge(const Edge* e) {
  free_edges_.push_back(const_cast<Edge*>(e));
}