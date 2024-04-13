NodeDebugInfo::NodeDebugInfo(
    StringPiece node_name, bool has_experimental_debug_info,
    const NodeDef_ExperimentalDebugInfo& experimental_debug_info)
    : name(node_name) {
  if (has_experimental_debug_info) {
    const auto& node_names = experimental_debug_info.original_node_names();
    original_node_names.assign(node_names.begin(), node_names.end());
    const auto& func_names = experimental_debug_info.original_func_names();
    original_func_names.assign(func_names.begin(), func_names.end());
  }
}