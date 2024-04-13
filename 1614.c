Status Graph::UpdateEdge(Node* new_src, int new_src_index, Node* dst,
                         int dst_index) {
  TF_RETURN_IF_ERROR(IsValidOutputTensor(new_src, new_src_index));
  TF_RETURN_IF_ERROR(IsValidInputTensor(dst, dst_index));
  const Edge* e = FindEdge(dst, dst_index);
  if (e == nullptr) {
    return errors::InvalidArgument("Couldn't find edge to ",
                                   FormatNodeForError(*dst));
  }
  RemoveEdge(e);
  AddEdge(new_src, new_src_index, dst, dst_index);
  dst->MaybeCopyOnWrite();
  (*dst->props_->node_def.mutable_input())[dst_index] =
      strings::StrCat(new_src->name(), ":", new_src_index);
  return Status::OK();
}