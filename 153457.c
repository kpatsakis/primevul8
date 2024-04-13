node_new_bag_if_else(Node* cond, Node* Then, Node* Else)
{
  Node* n;
  n = node_new_bag(BAG_IF_ELSE);
  CHECK_NULL_RETURN(n);

  NODE_BODY(n) = cond;
  BAG_(n)->te.Then = Then;
  BAG_(n)->te.Else = Else;
  return n;
}