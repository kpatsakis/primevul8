renumber_by_map(Node* node, GroupNumRemap* map)
{
  int r = 0;

  switch (NTYPE(node)) {
  case NT_LIST:
  case NT_ALT:
    do {
      r = renumber_by_map(NCAR(node), map);
    } while (r == 0 && IS_NOT_NULL(node = NCDR(node)));
    break;
  case NT_QTFR:
    r = renumber_by_map(NQTFR(node)->target, map);
    break;
  case NT_ENCLOSE:
    {
      EncloseNode* en = NENCLOSE(node);
      if (en->type == ENCLOSE_CONDITION)
	en->regnum = map[en->regnum].new_val;
      r = renumber_by_map(en->target, map);
    }
    break;

  case NT_BREF:
    r = renumber_node_backref(node, map);
    break;

  case NT_ANCHOR:
    if (NANCHOR(node)->target)
      r = renumber_by_map(NANCHOR(node)->target, map);
    break;

  default:
    break;
  }

  return r;
}