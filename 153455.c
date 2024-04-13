node_new_str(const UChar* s, const UChar* end)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  NODE_SET_TYPE(node, NODE_STRING);
  STR_(node)->flag     = 0;
  STR_(node)->s        = STR_(node)->buf;
  STR_(node)->end      = STR_(node)->buf;
  STR_(node)->capacity = 0;
  STR_(node)->case_min_len = 0;

  if (onig_node_str_cat(node, s, end)) {
    onig_node_free(node);
    return NULL;
  }
  return node;
}