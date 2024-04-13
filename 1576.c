void Graph::RemoveEdge(const Edge* e) {
  TF_DCHECK_OK(IsValidNode(e->src_)) << e->src_->DebugString();
  TF_DCHECK_OK(IsValidNode(e->dst_)) << e->dst_->DebugString();
  CHECK_EQ(e->src_->out_edges_.erase(e), size_t{1});
  CHECK_EQ(e->dst_->in_edges_.erase(e), size_t{1});
  CHECK_EQ(e, edges_[e->id_]);
  CHECK_GT(num_edges_, 0);

  edges_[e->id_] = nullptr;
  RecycleEdge(e);
  --num_edges_;

  if (!e->IsControlEdge()) {
    // This may clear the node type if enough edges are removed.
    e->dst_->RunForwardTypeInference();
  }
}