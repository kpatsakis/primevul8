str_node_can_be_split(StrNode* sn, OnigEncoding enc)
{
  if (sn->end > sn->s) {
    return ((enclen(enc, sn->s, sn->end) < sn->end - sn->s)  ?  1 : 0);
  }
  return 0;
}