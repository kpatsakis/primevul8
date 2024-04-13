void Graph::ReleaseNode(Node* node) {
  TF_DCHECK_OK(IsValidNode(node)) << node->DebugString();
  nodes_[node->id()] = nullptr;
  free_nodes_.push_back(node);
  --num_nodes_;
  node->Clear();
}