is_simple_one_char_repeat(Node* node, Node** rquant, Node** rbody,
                          int* is_possessive, ScanEnv* env)
{
  Node* quant;
  Node* body;

  *rquant = *rbody = 0;
  *is_possessive = 0;

  if (NODE_TYPE(node) == NODE_QUANT) {
    quant = node;
  }
  else {
    if (NODE_TYPE(node) == NODE_BAG) {
      BagNode* en = BAG_(node);
      if (en->type == BAG_STOP_BACKTRACK) {
        *is_possessive = 1;
        quant = NODE_BAG_BODY(en);
        if (NODE_TYPE(quant) != NODE_QUANT)
          return 0;
      }
      else
        return 0;
    }
    else
      return 0;
  }

  if (QUANT_(quant)->greedy == 0)
    return 0;

  body = NODE_BODY(quant);
  switch (NODE_TYPE(body)) {
  case NODE_STRING:
    {
      int len;
      StrNode* sn = STR_(body);
      UChar *s = sn->s;

      len = 0;
      while (s < sn->end) {
        s += enclen(env->enc, s);
        len++;
      }
      if (len != 1)
        return 0;
    }

  case NODE_CCLASS:
    break;

  default:
    return 0;
    break;
  }

  if (node != quant) {
    NODE_BODY(node) = 0;
    onig_node_free(node);
  }
  NODE_BODY(quant) = NULL_NODE;
  *rquant = quant;
  *rbody  = body;
  return 1;
}