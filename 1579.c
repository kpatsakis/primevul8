Status Graph::AddWhileInputHack(Node* new_src, int new_src_index, Node* dst) {
  if (!dst->IsWhileNode()) {
    return errors::Internal(
        "dst argument to AddWhileEdgeHack should be a While op, got: ",
        dst->DebugString());
  }
  TF_RETURN_IF_ERROR(IsValidOutputTensor(new_src, new_src_index));
  // Find the current number of data inputs. We'll add the new edge to the next
  // missing data input.
  int dst_index = 0;
  for (const Edge* edge : dst->in_edges()) {
    if (edge->IsControlEdge()) continue;
    ++dst_index;
  }
  TF_RETURN_IF_ERROR(IsValidInputTensor(dst, dst_index));
  AddEdge(new_src, new_src_index, dst, dst_index);
  dst->MaybeCopyOnWrite();
  dst->props_->node_def.add_input(
      strings::StrCat(new_src->name(), ":", new_src_index));
  return Status::OK();
}