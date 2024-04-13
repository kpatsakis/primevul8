onig_node_free(Node* node)
{
 start:
  if (IS_NULL(node)) return ;

#ifdef DEBUG_NODE_FREE
  fprintf(stderr, "onig_node_free: %p\n", node);
#endif

  switch (NODE_TYPE(node)) {
  case NODE_STRING:
    if (STR_(node)->capacity != 0 &&
        IS_NOT_NULL(STR_(node)->s) && STR_(node)->s != STR_(node)->buf) {
      xfree(STR_(node)->s);
    }
    break;

  case NODE_LIST:
  case NODE_ALT:
    onig_node_free(NODE_CAR(node));
    {
      Node* next_node = NODE_CDR(node);

      xfree(node);
      node = next_node;
      goto start;
    }
    break;

  case NODE_CCLASS:
    {
      CClassNode* cc = CCLASS_(node);

      if (cc->mbuf)
        bbuf_free(cc->mbuf);
    }
    break;

  case NODE_BACKREF:
    if (IS_NOT_NULL(BACKREF_(node)->back_dynamic))
      xfree(BACKREF_(node)->back_dynamic);
    break;

  case NODE_BAG:
    if (NODE_BODY(node))
      onig_node_free(NODE_BODY(node));

    {
      BagNode* en = BAG_(node);
      if (en->type == BAG_IF_ELSE) {
        onig_node_free(en->te.Then);
        onig_node_free(en->te.Else);
      }
    }
    break;

  case NODE_QUANT:
  case NODE_ANCHOR:
    if (NODE_BODY(node))
      onig_node_free(NODE_BODY(node));
    break;

  case NODE_CTYPE:
  case NODE_CALL:
  case NODE_GIMMICK:
    break;
  }

  xfree(node);
}