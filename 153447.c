node_new_str_crude(UChar* s, UChar* end)
{
  Node* node = node_new_str(s, end);
  CHECK_NULL_RETURN(node);
  NODE_STRING_SET_CRUDE(node);
  return node;
}