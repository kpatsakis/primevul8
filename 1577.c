std::string Graph::NewName(StringPiece prefix) {
  return strings::StrCat(prefix, "/_", name_counter_++);
}