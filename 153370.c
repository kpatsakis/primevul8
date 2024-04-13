node_new_bag(enum BagType type)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  NODE_SET_TYPE(node, NODE_BAG);
  BAG_(node)->type = type;

  switch (type) {
  case BAG_MEMORY:
    BAG_(node)->m.regnum       =  0;
    BAG_(node)->m.called_addr  = -1;
    BAG_(node)->m.entry_count  =  1;
    BAG_(node)->m.called_state =  0;
    break;

  case BAG_OPTION:
    BAG_(node)->o.options =  0;
    break;

  case BAG_STOP_BACKTRACK:
    break;

  case BAG_IF_ELSE:
    BAG_(node)->te.Then = 0;
    BAG_(node)->te.Else = 0;
    break;
  }

  BAG_(node)->opt_count = 0;
  return node;
}