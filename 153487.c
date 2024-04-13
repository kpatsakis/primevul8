str_node_split_last_char(Node* node, OnigEncoding enc)
{
  const UChar *p;
  Node* rn;
  StrNode* sn;

  sn = STR_(node);
  rn = NULL_NODE;
  if (sn->end > sn->s) {
    p = onigenc_get_prev_char_head(enc, sn->s, sn->end);
    if (p && p > sn->s) { /* can be split. */
      rn = node_new_str(p, sn->end);
      CHECK_NULL_RETURN(rn);
      if (NODE_STRING_IS_CRUDE(node))
        NODE_STRING_SET_CRUDE(rn);

      sn->end = (UChar* )p;
    }
  }
  return rn;
}