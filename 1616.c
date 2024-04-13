AttrValue* Node::AddAttrHelper(const std::string& name) {
  MaybeCopyOnWrite();
  return &((*props_->node_def.mutable_attr())[name]);
}