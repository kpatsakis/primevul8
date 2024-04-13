Graph::Graph(const OpRegistryInterface* ops)
    : ops_(ops, FunctionDefLibrary()),
      versions_(new VersionDef),
      arena_(8 << 10 /* 8kB */) {
  versions_->set_producer(TF_GRAPH_DEF_VERSION);
  versions_->set_min_consumer(TF_GRAPH_DEF_VERSION_MIN_CONSUMER);

  // Initialize the name interning table for assigned_device_name.
  device_names_.push_back("");
  DCHECK_EQ(0, InternDeviceName(""));

  // Source and sink have no endpoints, just control edges.
  NodeDef def;
  def.set_name("_SOURCE");
  def.set_op("NoOp");
  Status status;
  Node* source = AddNode(def, &status);
  TF_CHECK_OK(status);
  CHECK_EQ(source->id(), kSourceId);

  def.set_name("_SINK");
  Node* sink = AddNode(def, &status);
  TF_CHECK_OK(status);
  CHECK_EQ(sink->id(), kSinkId);

  AddControlEdge(source, sink);
}