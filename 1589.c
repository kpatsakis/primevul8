int Graph::InternDeviceName(const std::string& device_name) {
  // Special case, very common.  Also, this allows us to use a single map
  // lookup below, instead of two.  The 'if (index_cell > 0)' test below
  // relies on this check.
  if (device_name.empty()) {
    return 0;
  }

  int& index_cell = device_names_map_[device_name];
  if (index_cell > 0) {
    return index_cell;
  }

  const int index = device_names_map_.size();
  index_cell = index;
  device_names_.push_back(device_name);
  return index;
}