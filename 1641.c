Status Graph::AddFunctionLibrary(const FunctionDefLibrary& fdef_lib) {
  // Need a new-enough consumer to support the functions we add to the graph.
  if (fdef_lib.function_size() > 0 && versions_->min_consumer() < 12) {
    versions_->set_min_consumer(12);
  }
  return ops_.AddLibrary(fdef_lib);
}