node_drop_group(Node* group)
{
  Node* content;

  content = NODE_CAR(group);
  NODE_CAR(group) = NULL_NODE;
  onig_node_free(group);
  return content;
}