void Graph::ToGraphDef(GraphDef* graph_def) const {
  ToGraphDefSubRange(graph_def, 0);
}