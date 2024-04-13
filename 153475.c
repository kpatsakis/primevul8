node_new(void)
{
  Node* node;

  node = (Node* )xmalloc(sizeof(Node));
  CHECK_NULL_RETURN(node);
  xmemset(node, 0, sizeof(*node));

#ifdef DEBUG_NODE_FREE
  fprintf(stderr, "node_new: %p\n", node);
#endif
  return node;
}