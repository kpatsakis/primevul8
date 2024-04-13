GraphDef Graph::ToGraphDefDebug() const {
  GraphDef ret;
  ToGraphDef(&ret);
  return ret;
}