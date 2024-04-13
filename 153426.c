make_absent_engine(Node** node, int pre_save_right_id, Node* absent,
                   Node* step_one, int lower, int upper, int possessive,
                   int is_range_cutter, ScanEnv* env)
{
  int r;
  int i;
  int id;
  Node* x;
  Node* ns[4];

  for (i = 0; i < 4; i++) ns[i] = NULL_NODE;

  ns[1] = absent;
  ns[3] = step_one; /* for err */
  r = node_new_save_gimmick(&ns[0], SAVE_S, env);
  if (r != 0) goto err;

  id = GIMMICK_(ns[0])->id;
  r = node_new_update_var_gimmick(&ns[2], UPDATE_VAR_RIGHT_RANGE_FROM_S_STACK,
                                  id, env);
  if (r != 0) goto err;

  r = node_new_fail(&ns[3], env);
  if (r != 0) goto err;

  x = make_list(4, ns);
  if (IS_NULL(x)) goto err0;

  ns[0] = x;
  ns[1] = step_one;
  ns[2] = ns[3] = NULL_NODE;

  x = make_alt(2, ns);
  if (IS_NULL(x)) goto err0;

  ns[0] = x;

  x = node_new_quantifier(lower, upper, FALSE);
  if (IS_NULL(x)) goto err0;

  NODE_BODY(x) = ns[0];
  ns[0] = x;

  if (possessive != 0) {
    x = node_new_bag(BAG_STOP_BACKTRACK);
    if (IS_NULL(x)) goto err0;

    NODE_BODY(x) = ns[0];
    ns[0] = x;
  }

  r = node_new_update_var_gimmick(&ns[1], UPDATE_VAR_RIGHT_RANGE_FROM_STACK,
                                  pre_save_right_id, env);
  if (r != 0) goto err;

  r = node_new_fail(&ns[2], env);
  if (r != 0) goto err;

  x = make_list(2, ns + 1);
  if (IS_NULL(x)) goto err0;

  ns[1] = x; ns[2] = NULL_NODE;

  x = make_alt(2, ns);
  if (IS_NULL(x)) goto err0;

  if (is_range_cutter != FALSE)
    NODE_STATUS_ADD(x, SUPER);

  *node = x;
  return ONIG_NORMAL;

 err0:
  r = ONIGERR_MEMORY;
 err:
  for (i = 0; i < 4; i++) onig_node_free(ns[i]);
  return r;
}