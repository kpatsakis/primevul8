str_node_can_be_split(Node* node, OnigEncoding enc)
{
  StrNode* sn = STR_(node);
  if (sn->end > sn->s) {
    return ((enclen(enc, sn->s) < sn->end - sn->s)  ?  1 : 0);
  }
  return 0;
}