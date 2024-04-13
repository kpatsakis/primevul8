make_absent_tree_for_simple_one_char_repeat(Node** node, Node* absent, Node* quant,
                                            Node* body, int possessive, ScanEnv* env)
{
  int r;
  int i;
  int id1;
  int lower, upper;
  Node* x;
  Node* ns[4];

  *node = NULL_NODE;
  r = ONIGERR_MEMORY;
  ns[0] = ns[1] = NULL_NODE;
  ns[2] = body, ns[3] = absent;

  lower = QUANT_(quant)->lower;
  upper = QUANT_(quant)->upper;
  onig_node_free(quant);

  r = node_new_save_gimmick(&ns[0], SAVE_RIGHT_RANGE, env);
  if (r != 0) goto err;

  id1 = GIMMICK_(ns[0])->id;

  r = make_absent_engine(&ns[1], id1, absent, body, lower, upper, possessive,
                         FALSE, env);
  if (r != 0) goto err;

  ns[2] = ns[3] = NULL_NODE;

  r = node_new_update_var_gimmick(&ns[2], UPDATE_VAR_RIGHT_RANGE_FROM_STACK,
                                  id1, env);
  if (r != 0) goto err;

  x = make_list(3, ns);
  if (IS_NULL(x)) goto err0;

  *node = x;
  return ONIG_NORMAL;

 err0:
  r = ONIGERR_MEMORY;
 err:
  for (i = 0; i < 4; i++) onig_node_free(ns[i]);
  return r;
}