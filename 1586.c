void Graph::RemoveControlEdge(const Edge* e) {
  if (!e->src_->IsSource() && !e->dst_->IsSink()) {
    e->dst_->MaybeCopyOnWrite();
    std::string e_src_name = strings::StrCat("^", e->src_->name());
    auto* inputs = e->dst_->props_->node_def.mutable_input();
    for (auto it = inputs->begin(); it != inputs->end(); ++it) {
      if (*it == e_src_name) {
        inputs->erase(it);
        break;
      }
    }
  }
  RemoveEdge(e);
}