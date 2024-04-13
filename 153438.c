make_range_clear(Node** node, ScanEnv* env)
{
  int r;
  int id;
  Node* save;
  Node* x;
  Node* ns[2];

  *node = NULL_NODE;
  save = ns[0] = ns[1] = NULL_NODE;

  r = node_new_save_gimmick(&save, SAVE_RIGHT_RANGE, env);
  if (r != 0) goto err;

  id = GIMMICK_(save)->id;
  r = node_new_update_var_gimmick(&ns[0], UPDATE_VAR_RIGHT_RANGE_FROM_STACK,
                                  id, env);
  if (r != 0) goto err;

  r = node_new_fail(&ns[1], env);
  if (r != 0) goto err;

  x = make_list(2, ns);
  if (IS_NULL(x)) goto err0;

  ns[0] = NULL_NODE; ns[1] = x;

#define ID_NOT_USED_DONT_CARE_ME   0

  r = node_new_update_var_gimmick(&ns[0], UPDATE_VAR_RIGHT_RANGE_INIT,
                                  ID_NOT_USED_DONT_CARE_ME, env);
  if (r != 0) goto err;

  x = make_alt(2, ns);
  if (IS_NULL(x)) goto err0;

  NODE_STATUS_ADD(x, SUPER);

  ns[0] = save;
  ns[1] = x;
  save = NULL_NODE;
  x = make_list(2, ns);
  if (IS_NULL(x)) goto err0;

  *node = x;
  return ONIG_NORMAL;

 err0:
  r = ONIGERR_MEMORY;
 err:
  onig_node_free(save);
  onig_node_free(ns[0]);
  onig_node_free(ns[1]);
  return r;
}