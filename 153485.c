node_new_group(Node* content)
{
  Node* node;

  node = node_new();
  CHECK_NULL_RETURN(node);
  NODE_SET_TYPE(node, NODE_LIST);
  NODE_CAR(node) = content;
  NODE_CDR(node) = NULL_NODE;

  return node;
}