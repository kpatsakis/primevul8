make_call_zero_body(Node* node, ScanEnv* env, Node** rnode)
{
  int r;

  Node* x = node_new_memory(0 /* 0: is not named */);
  CHECK_NULL_RETURN_MEMERR(x);

  NODE_BODY(x) = node;
  BAG_(x)->m.regnum = 0;
  r = scan_env_set_mem_node(env, 0, x);
  if (r != 0) {
    onig_node_free(x);
    return r;
  }

  *rnode = x;
  return 0;
}