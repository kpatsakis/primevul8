next_setup(Node* node, Node* next_node, int in_root, regex_t* reg)
{
  int type;

 retry:
  type = NTYPE(node);
  if (type == NT_QTFR) {
    QtfrNode* qn = NQTFR(node);
    if (qn->greedy && IS_REPEAT_INFINITE(qn->upper)) {
#ifdef USE_QTFR_PEEK_NEXT
      Node* n = get_head_value_node(next_node, 1, reg);
      /* '\0': for UTF-16BE etc... */
      if (IS_NOT_NULL(n) && NSTR(n)->s[0] != '\0') {
	qn->next_head_exact = n;
      }
#endif
      /* automatic possessification a*b ==> (?>a*)b */
      if (qn->lower <= 1) {
	int ttype = NTYPE(qn->target);
	if (IS_NODE_TYPE_SIMPLE(ttype)) {
	  Node *x, *y;
	  x = get_head_value_node(qn->target, 0, reg);
	  if (IS_NOT_NULL(x)) {
	    y = get_head_value_node(next_node,  0, reg);
	    if (IS_NOT_NULL(y) && is_not_included(x, y, reg)) {
	      Node* en = onig_node_new_enclose(ENCLOSE_STOP_BACKTRACK);
	      CHECK_NULL_RETURN_MEMERR(en);
	      SET_ENCLOSE_STATUS(en, NST_STOP_BT_SIMPLE_REPEAT);
	      swap_node(node, en);
	      NENCLOSE(node)->target = en;
	    }
	  }
	}
      }

#ifndef ONIG_DONT_OPTIMIZE
      if (NTYPE(node) == NT_QTFR && /* the type may be changed by above block */
	  in_root && /* qn->lower == 0 && */
	  NTYPE(qn->target) == NT_CANY &&
	  ! IS_MULTILINE(reg->options)) {
	/* implicit anchor: /.*a/ ==> /(?:^|\G).*a/ */
	Node *np;
	np = onig_node_new_list(NULL_NODE, NULL_NODE);
	CHECK_NULL_RETURN_MEMERR(np);
	swap_node(node, np);
	NCDR(node) = onig_node_new_list(np, NULL_NODE);
	if (IS_NULL(NCDR(node))) {
	  onig_node_free(np);
	  return ONIGERR_MEMORY;
	}
	np = onig_node_new_anchor(ANCHOR_ANYCHAR_STAR);   /* (?:^|\G) */
	CHECK_NULL_RETURN_MEMERR(np);
	NCAR(node) = np;
      }
#endif
    }
  }
  else if (type == NT_ENCLOSE) {
    EncloseNode* en = NENCLOSE(node);
    in_root = 0;
    if (en->type == ENCLOSE_MEMORY) {
      node = en->target;
      goto retry;
    }
  }
  return 0;
}