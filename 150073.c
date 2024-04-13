node_new_str_raw(UChar* s, UChar* end)
{
  Node* node = node_new_str(s, end);
  if (IS_NOT_NULL(node))
    NSTRING_SET_RAW(node);
  return node;
}