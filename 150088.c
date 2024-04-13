onig_node_free(Node* node)
{
 start:
  if (IS_NULL(node)) return ;

  switch (NTYPE(node)) {
  case NT_STR:
    if (NSTR(node)->capa != 0 &&
	IS_NOT_NULL(NSTR(node)->s) && NSTR(node)->s != NSTR(node)->buf) {
      xfree(NSTR(node)->s);
    }
    break;

  case NT_LIST:
  case NT_ALT:
    onig_node_free(NCAR(node));
    {
      Node* next_node = NCDR(node);

      xfree(node);
      node = next_node;
      goto start;
    }
    break;

  case NT_CCLASS:
    {
      CClassNode* cc = NCCLASS(node);

      if (cc->mbuf)
	bbuf_free(cc->mbuf);
    }
    break;

  case NT_QTFR:
    if (NQTFR(node)->target)
      onig_node_free(NQTFR(node)->target);
    break;

  case NT_ENCLOSE:
    if (NENCLOSE(node)->target)
      onig_node_free(NENCLOSE(node)->target);
    break;

  case NT_BREF:
    if (IS_NOT_NULL(NBREF(node)->back_dynamic))
      xfree(NBREF(node)->back_dynamic);
    break;

  case NT_ANCHOR:
    if (NANCHOR(node)->target)
      onig_node_free(NANCHOR(node)->target);
    break;
  }

  xfree(node);
}