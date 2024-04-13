node_new_ctype(int type, int not, int ascii_range)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  SET_NTYPE(node, NT_CTYPE);
  NCTYPE(node)->ctype = type;
  NCTYPE(node)->not   = not;
  NCTYPE(node)->ascii_range = ascii_range;
  return node;
}