node_new_memory(int is_named)
{
  Node* node = node_new_bag(BAG_MEMORY);
  CHECK_NULL_RETURN(node);
  if (is_named != 0)
    NODE_STATUS_ADD(node, NAMED_GROUP);

  return node;
}