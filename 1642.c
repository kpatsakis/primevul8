std::unique_ptr<Graph> Graph::Clone() {
  std::unique_ptr<Graph> new_graph(new Graph(flib_def()));
  new_graph->Copy(*this);
  return new_graph;
}