const Edge* Graph::AddControlEdge(Node* source, Node* dest,
                                  bool allow_duplicates) {
  if (!allow_duplicates) {
    for (const Edge* edge : dest->in_edges()) {
      if (edge->IsControlEdge() && edge->src() == source) {
        // The requested edge already exists.
        return nullptr;
      }
    }
  }
  // Modify dest's NodeDef if necessary.
  if (!source->IsSource() && !dest->IsSink() && !allow_duplicates) {
    // Check if this input is already in dest's NodeDef.
    const std::string new_input = strings::StrCat("^", source->name());
    bool input_exists = false;
    for (const std::string& input : dest->props_->node_def.input()) {
      if (input == new_input) {
        input_exists = true;
        break;
      }
    }
    if (!input_exists) {
      dest->MaybeCopyOnWrite();
      dest->props_->node_def.add_input(new_input);
    }
  }
  return AddEdge(source, kControlSlot, dest, kControlSlot);
}