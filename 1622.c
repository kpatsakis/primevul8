Node* Graph::CopyNode(const Node* node) {
  DCHECK(!node->IsSource());
  DCHECK(!node->IsSink());
  Node* copy = AllocateNode(node->props_, node, node->class_);
  copy->set_assigned_device_name(node->assigned_device_name());

  // Since the OpDef of a function may be owned by the Graph that owns 'node',
  // relookup the OpDef in the target graph. If it differs, then clone the
  // node properties with the updated OpDef.
  const OpDef* op_def;
  TF_CHECK_OK(ops_.LookUpOpDef(node->type_string(), &op_def));
  if (op_def != node->props_->op_def) {
    copy->MaybeCopyOnWrite();
    copy->props_->op_def = op_def;
  }
  copy->SetStackTrace(node->GetStackTrace());

  return copy;
}