next_setup(Node* node, Node* next_node, regex_t* reg)
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
      /* automatic posseivation a*b ==> (?>a*)b */
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
    }
  }
  else if (type == NT_ENCLOSE) {
    EncloseNode* en = NENCLOSE(node);
    if (en->type == ENCLOSE_MEMORY) {
      node = en->target;
      goto retry;
    }
  }
  return 0;
}