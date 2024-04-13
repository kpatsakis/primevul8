make_absent_tree(Node** node, Node* absent, Node* expr, int is_range_cutter,
                 ScanEnv* env)
{
  int r;
  int i;
  int id1, id2;
  int possessive;
  Node* x;
  Node* ns[7];

  r = ONIGERR_MEMORY;
  for (i = 0; i < 7; i++) ns[i] = NULL_NODE;
  ns[4] = expr; ns[5] = absent;

  if (is_range_cutter == 0) {
    Node* quant;
    Node* body;

    if (expr == NULL_NODE) {
      /* default expr \O* */
      quant = node_new_quantifier(0, INFINITE_REPEAT, FALSE);
      if (IS_NULL(quant)) goto err0;

      r = node_new_true_anychar(&body, env);
      if (r != 0) {
        onig_node_free(quant);
        goto err;
      }
      possessive = 0;
      goto simple;
    }
    else {
      if (is_simple_one_char_repeat(expr, &quant, &body, &possessive, env)) {
      simple:
        r = make_absent_tree_for_simple_one_char_repeat(node, absent, quant,
                                                        body, possessive, env);
        if (r != 0) {
          ns[4] = NULL_NODE;
          onig_node_free(quant);
          onig_node_free(body);
          goto err;
        }

        return ONIG_NORMAL;
      }
    }
  }

  r = node_new_save_gimmick(&ns[0], SAVE_RIGHT_RANGE, env);
  if (r != 0) goto err;

  id1 = GIMMICK_(ns[0])->id;

  r = node_new_save_gimmick(&ns[1], SAVE_S, env);
  if (r != 0) goto err;

  id2 = GIMMICK_(ns[1])->id;

  r = node_new_true_anychar(&ns[3], env);
  if (r != 0) goto err;

  possessive = 1;
  r = make_absent_engine(&ns[2], id1, absent, ns[3], 0, INFINITE_REPEAT,
                         possessive, is_range_cutter, env);
  if (r != 0) goto err;

  ns[3] = NULL_NODE;
  ns[5] = NULL_NODE;

  r = node_new_update_var_gimmick(&ns[3], UPDATE_VAR_S_FROM_STACK, id2, env);
  if (r != 0) goto err;

  if (is_range_cutter != 0) {
    x = make_list(4, ns);
    if (IS_NULL(x)) goto err0;
  }
  else {
    r = make_absent_tail(&ns[5], &ns[6], id1, env);
    if (r != 0) goto err;

    x = make_list(7, ns);
    if (IS_NULL(x)) goto err0;
  }

  *node = x;
  return ONIG_NORMAL;

 err0:
  r = ONIGERR_MEMORY;
 err:
  for (i = 0; i < 7; i++) onig_node_free(ns[i]);
  return r;
}