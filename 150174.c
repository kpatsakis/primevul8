expand_case_fold_make_rem_string(Node** rnode, UChar *s, UChar *end,
				 regex_t* reg)
{
  int r;
  Node *node;

  node = onig_node_new_str(s, end);
  if (IS_NULL(node)) return ONIGERR_MEMORY;

  r = update_string_node_case_fold(reg, node);
  if (r != 0) {
    onig_node_free(node);
    return r;
  }

  NSTRING_SET_AMBIG(node);
  NSTRING_SET_DONT_GET_OPT_INFO(node);
  *rnode = node;
  return 0;
}