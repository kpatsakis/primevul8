parse_branch(Node** top, PToken* tok, int term, UChar** src, UChar* end,
             ScanEnv* env, int group_head)
{
  int r;
  Node *node, **headp;

  *top = NULL;
  INC_PARSE_DEPTH(env->parse_depth);

  r = parse_exp(&node, tok, term, src, end, env, group_head);
  if (r < 0) {
    onig_node_free(node);
    return r;
  }

  if (r == TK_EOT || r == term || r == TK_ALT) {
    *top = node;
  }
  else {
    *top = node_new_list(node, NULL);
    if (IS_NULL(*top)) {
      onig_node_free(node);
      return ONIGERR_MEMORY;
    }

    headp = &(NODE_CDR(*top));
    while (r != TK_EOT && r != term && r != TK_ALT) {
      r = parse_exp(&node, tok, term, src, end, env, FALSE);
      if (r < 0) {
        onig_node_free(node);
        return r;
      }

      if (NODE_TYPE(node) == NODE_LIST) {
        *headp = node;
        while (IS_NOT_NULL(NODE_CDR(node))) node = NODE_CDR(node);
        headp = &(NODE_CDR(node));
      }
      else {
        *headp = node_new_list(node, NULL);
        headp = &(NODE_CDR(*headp));
      }
    }
  }

  DEC_PARSE_DEPTH(env->parse_depth);
  return r;
}