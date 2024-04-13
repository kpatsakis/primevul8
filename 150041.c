compile_quantifier_node(QtfrNode* qn, regex_t* reg)
{
  int i, r, mod_tlen;
  int infinite = IS_REPEAT_INFINITE(qn->upper);
  int empty_info = qn->target_empty_info;
  int tlen = compile_length_tree(qn->target, reg);

  if (tlen < 0) return tlen;

  if (is_anychar_star_quantifier(qn)) {
    r = compile_tree_n_times(qn->target, qn->lower, reg);
    if (r) return r;
    if (IS_NOT_NULL(qn->next_head_exact)) {
      if (IS_MULTILINE(reg->options))
	r = add_opcode(reg, OP_ANYCHAR_ML_STAR_PEEK_NEXT);
      else
	r = add_opcode(reg, OP_ANYCHAR_STAR_PEEK_NEXT);
      if (r) return r;
      return add_bytes(reg, NSTR(qn->next_head_exact)->s, 1);
    }
    else {
      if (IS_MULTILINE(reg->options))
	return add_opcode(reg, OP_ANYCHAR_ML_STAR);
      else
	return add_opcode(reg, OP_ANYCHAR_STAR);
    }
  }

  if (empty_info != 0)
    mod_tlen = tlen + (SIZE_OP_NULL_CHECK_START + SIZE_OP_NULL_CHECK_END);
  else
    mod_tlen = tlen;

  if (infinite &&
      (qn->lower <= 1 || tlen * qn->lower <= QUANTIFIER_EXPAND_LIMIT_SIZE)) {
    if (qn->lower == 1 && tlen > QUANTIFIER_EXPAND_LIMIT_SIZE) {
      if (qn->greedy) {
#ifdef USE_OP_PUSH_OR_JUMP_EXACT
	if (IS_NOT_NULL(qn->head_exact))
	  r = add_opcode_rel_addr(reg, OP_JUMP, SIZE_OP_PUSH_OR_JUMP_EXACT1);
	else
#endif
	if (IS_NOT_NULL(qn->next_head_exact))
	  r = add_opcode_rel_addr(reg, OP_JUMP, SIZE_OP_PUSH_IF_PEEK_NEXT);
	else
	  r = add_opcode_rel_addr(reg, OP_JUMP, SIZE_OP_PUSH);
      }
      else {
	r = add_opcode_rel_addr(reg, OP_JUMP, SIZE_OP_JUMP);
      }
      if (r) return r;
    }
    else {
      r = compile_tree_n_times(qn->target, qn->lower, reg);
      if (r) return r;
    }

    if (qn->greedy) {
#ifdef USE_OP_PUSH_OR_JUMP_EXACT
      if (IS_NOT_NULL(qn->head_exact)) {
	r = add_opcode_rel_addr(reg, OP_PUSH_OR_JUMP_EXACT1,
			     mod_tlen + SIZE_OP_JUMP);
	if (r) return r;
	add_bytes(reg, NSTR(qn->head_exact)->s, 1);
	r = compile_tree_empty_check(qn->target, reg, empty_info);
	if (r) return r;
	r = add_opcode_rel_addr(reg, OP_JUMP,
	-(mod_tlen + (int )SIZE_OP_JUMP + (int )SIZE_OP_PUSH_OR_JUMP_EXACT1));
      }
      else
#endif
      if (IS_NOT_NULL(qn->next_head_exact)) {
	r = add_opcode_rel_addr(reg, OP_PUSH_IF_PEEK_NEXT,
				mod_tlen + SIZE_OP_JUMP);
	if (r) return r;
	add_bytes(reg, NSTR(qn->next_head_exact)->s, 1);
	r = compile_tree_empty_check(qn->target, reg, empty_info);
	if (r) return r;
	r = add_opcode_rel_addr(reg, OP_JUMP,
          -(mod_tlen + (int )SIZE_OP_JUMP + (int )SIZE_OP_PUSH_IF_PEEK_NEXT));
      }
      else {
	r = add_opcode_rel_addr(reg, OP_PUSH, mod_tlen + SIZE_OP_JUMP);
	if (r) return r;
	r = compile_tree_empty_check(qn->target, reg, empty_info);
	if (r) return r;
	r = add_opcode_rel_addr(reg, OP_JUMP,
		     -(mod_tlen + (int )SIZE_OP_JUMP + (int )SIZE_OP_PUSH));
      }
    }
    else {
      r = add_opcode_rel_addr(reg, OP_JUMP, mod_tlen);
      if (r) return r;
      r = compile_tree_empty_check(qn->target, reg, empty_info);
      if (r) return r;
      r = add_opcode_rel_addr(reg, OP_PUSH, -(mod_tlen + (int )SIZE_OP_PUSH));
    }
  }
  else if (qn->upper == 0 && qn->is_refered != 0) { /* /(?<n>..){0}/ */
    r = add_opcode_rel_addr(reg, OP_JUMP, tlen);
    if (r) return r;
    r = compile_tree(qn->target, reg);
  }
  else if (!infinite && qn->greedy &&
           (qn->upper == 1 || (tlen + SIZE_OP_PUSH) * qn->upper
                                  <= QUANTIFIER_EXPAND_LIMIT_SIZE)) {
    int n = qn->upper - qn->lower;

    r = compile_tree_n_times(qn->target, qn->lower, reg);
    if (r) return r;

    for (i = 0; i < n; i++) {
      r = add_opcode_rel_addr(reg, OP_PUSH,
			   (n - i) * tlen + (n - i - 1) * SIZE_OP_PUSH);
      if (r) return r;
      r = compile_tree(qn->target, reg);
      if (r) return r;
    }
  }
  else if (!qn->greedy && qn->upper == 1 && qn->lower == 0) { /* '??' */
    r = add_opcode_rel_addr(reg, OP_PUSH, SIZE_OP_JUMP);
    if (r) return r;
    r = add_opcode_rel_addr(reg, OP_JUMP, tlen);
    if (r) return r;
    r = compile_tree(qn->target, reg);
  }
  else {
    r = compile_range_repeat_node(qn, mod_tlen, empty_info, reg);
  }
  return r;
}