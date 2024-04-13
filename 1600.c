void Graph::ToGraphDefSubRange(GraphDef* graph_def, int from_node_id) const {
  graph_def->Clear();
  *graph_def->mutable_versions() = versions();
  *graph_def->mutable_library() = ops_.ToProto();

  graph_def->mutable_node()->Reserve(std::max(1, num_nodes() - from_node_id));

  std::vector<const Edge*>
      inputs;  // Construct this outside the loop for speed.
  for (auto id = from_node_id; id < num_node_ids(); ++id) {
    const Node* node = FindNodeId(id);
    if (node == nullptr || !node->IsOp()) continue;
    NodeDef* node_def = graph_def->add_node();
    *node_def = node->def();

    // Use the node's assigned device, if any, instead of the device requested
    // in the NodeDef.
    if (!node->assigned_device_name().empty()) {
      node_def->set_device(node->assigned_device_name());
    }

    // Get the inputs for this Node.  We make sure control inputs are
    // after data inputs, as required by GraphDef.
    inputs.clear();
    inputs.resize(node->num_inputs(), nullptr);
    for (const Edge* edge : node->in_edges()) {
      if (edge->IsControlEdge()) {
        inputs.push_back(edge);
      } else {
        DCHECK(edge->dst_input() < inputs.size())
            << "Edge " << edge->DebugString()
            << " is overflowing the expected number of inputs ("
            << node->num_inputs() << ") for node " << node->DebugString();
        CHECK(inputs[edge->dst_input()] == nullptr)
            << "Edge " << edge->src()->name() << "->" << edge->dst()->name()
            << " conflicts with pre-existing input edge "
            << inputs[edge->dst_input()]->src()->name() << "->"
            << inputs[edge->dst_input()]->dst()->name();

        inputs[edge->dst_input()] = edge;
      }
    }
    // Sort the control inputs for more predictable serialization.
    std::sort(inputs.begin() + node->num_inputs(), inputs.end(),
              [](const Edge* a, const Edge* b) -> bool {
                return a->src()->name() < b->src()->name();
              });
    node_def->clear_input();
    node_def->mutable_input()->Reserve(inputs.size());

    for (size_t i = 0; i < inputs.size(); ++i) {
      const Edge* edge = inputs[i];
      if (edge == nullptr) {
        if (i < node->requested_inputs().size()) {
          node_def->add_input(node->requested_inputs()[i]);
        } else {
          node_def->add_input("");
        }
      } else {
        const Node* src = edge->src();
        if (!src->IsOp()) continue;
        AddInput(node_def, src->name(), edge->src_output());
      }
    }
  }
}