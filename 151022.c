compile_enclose_node(EncloseNode* node, regex_t* reg)
{
  int r, len;

  if (node->type == ENCLOSE_OPTION)
    return compile_option_node(node, reg);

  switch (node->type) {
  case ENCLOSE_MEMORY:
#ifdef USE_SUBEXP_CALL
    if (IS_ENCLOSE_CALLED(node)) {
      r = add_opcode(reg, OP_CALL);
      if (r) return r;
      node->call_addr = BBUF_GET_OFFSET_POS(reg) + SIZE_ABSADDR + SIZE_OP_JUMP;
      node->state |= NST_ADDR_FIXED;
      r = add_abs_addr(reg, (int )node->call_addr);
      if (r) return r;
      len = compile_length_tree(node->target, reg);
      len += (SIZE_OP_MEMORY_START_PUSH + SIZE_OP_RETURN);
      if (BIT_STATUS_AT(reg->bt_mem_end, node->regnum))
	len += (IS_ENCLOSE_RECURSION(node)
		? SIZE_OP_MEMORY_END_PUSH_REC : SIZE_OP_MEMORY_END_PUSH);
      else
	len += (IS_ENCLOSE_RECURSION(node)
		? SIZE_OP_MEMORY_END_REC : SIZE_OP_MEMORY_END);

      r = add_opcode_rel_addr(reg, OP_JUMP, len);
      if (r) return r;
    }
#endif
    if (BIT_STATUS_AT(reg->bt_mem_start, node->regnum))
      r = add_opcode(reg, OP_MEMORY_START_PUSH);
    else
      r = add_opcode(reg, OP_MEMORY_START);
    if (r) return r;
    r = add_mem_num(reg, node->regnum);
    if (r) return r;
    r = compile_tree(node->target, reg);
    if (r) return r;
#ifdef USE_SUBEXP_CALL
    if (IS_ENCLOSE_CALLED(node)) {
      if (BIT_STATUS_AT(reg->bt_mem_end, node->regnum))
	r = add_opcode(reg, (IS_ENCLOSE_RECURSION(node)
			     ? OP_MEMORY_END_PUSH_REC : OP_MEMORY_END_PUSH));
      else
	r = add_opcode(reg, (IS_ENCLOSE_RECURSION(node)
			     ? OP_MEMORY_END_REC : OP_MEMORY_END));

      if (r) return r;
      r = add_mem_num(reg, node->regnum);
      if (r) return r;
      r = add_opcode(reg, OP_RETURN);
    }
    else
#endif
    {
      if (BIT_STATUS_AT(reg->bt_mem_end, node->regnum))
	r = add_opcode(reg, OP_MEMORY_END_PUSH);
      else
	r = add_opcode(reg, OP_MEMORY_END);
      if (r) return r;
      r = add_mem_num(reg, node->regnum);
    }
    break;

  case ENCLOSE_STOP_BACKTRACK:
    if (IS_ENCLOSE_STOP_BT_SIMPLE_REPEAT(node)) {
      QtfrNode* qn = NQTFR(node->target);
      r = compile_tree_n_times(qn->target, qn->lower, reg);
      if (r) return r;

      len = compile_length_tree(qn->target, reg);
      if (len < 0) return len;

      r = add_opcode_rel_addr(reg, OP_PUSH, len + SIZE_OP_POP + SIZE_OP_JUMP);
      if (r) return r;
      r = compile_tree(qn->target, reg);
      if (r) return r;
      r = add_opcode(reg, OP_POP);
      if (r) return r;
      r = add_opcode_rel_addr(reg, OP_JUMP,
	 -((int )SIZE_OP_PUSH + len + (int )SIZE_OP_POP + (int )SIZE_OP_JUMP));
    }
    else {
      r = add_opcode(reg, OP_PUSH_STOP_BT);
      if (r) return r;
      r = compile_tree(node->target, reg);
      if (r) return r;
      r = add_opcode(reg, OP_POP_STOP_BT);
    }
    break;

  default:
    return ONIGERR_TYPE_BUG;
    break;
  }

  return r;
}