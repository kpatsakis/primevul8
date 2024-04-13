swap_node(Node* a, Node* b)
{
  Node c;
  c = *a; *a = *b; *b = c;

  if (NTYPE(a) == NT_STR) {
    StrNode* sn = NSTR(a);
    if (sn->capa == 0) {
      size_t len = sn->end - sn->s;
      sn->s   = sn->buf;
      sn->end = sn->s + len;
    }
  }

  if (NTYPE(b) == NT_STR) {
    StrNode* sn = NSTR(b);
    if (sn->capa == 0) {
      size_t len = sn->end - sn->s;
      sn->s   = sn->buf;
      sn->end = sn->s + len;
    }
  }
}