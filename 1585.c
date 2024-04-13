std::string Node::DebugString() const {
  std::string ret = strings::StrCat("{name:'", name(), "' id:", id_);
  if (IsSource()) {
    strings::StrAppend(&ret, " source}");
  } else if (IsSink()) {
    strings::StrAppend(&ret, " sink}");
  } else {
    strings::StrAppend(&ret, " op device:", "{requested: '", requested_device(),
                       "', assigned: '", assigned_device_name(), "'}", " def:{",
                       SummarizeNode(*this), "}}");
  }
  return ret;
}