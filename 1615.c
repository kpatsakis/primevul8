void Graph::RemoveNode(Node* node) {
  TF_DCHECK_OK(IsValidNode(node)) << node->DebugString();
  DCHECK(!node->IsSource());
  DCHECK(!node->IsSink());

  // Remove any edges involving this node.
  for (const Edge* e : node->in_edges_) {
    CHECK_EQ(e->src_->out_edges_.erase(e), size_t{1});
    edges_[e->id_] = nullptr;
    RecycleEdge(e);
    --num_edges_;
  }
  node->in_edges_.clear();
  for (const Edge* e : node->out_edges_) {
    CHECK_EQ(e->dst_->in_edges_.erase(e), size_t{1});
    edges_[e->id_] = nullptr;
    RecycleEdge(e);
    --num_edges_;
  }
  node->out_edges_.clear();
  ReleaseNode(node);
}