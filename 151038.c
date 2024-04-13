compile_length_enclose_node(EncloseNode* node, regex_t* reg)
{
  int len;
  int tlen;

  if (node->type == ENCLOSE_OPTION)
    return compile_length_option_node(node, reg);

  if (node->target) {
    tlen = compile_length_tree(node->target, reg);
    if (tlen < 0) return tlen;
  }
  else
    tlen = 0;

  switch (node->type) {
  case ENCLOSE_MEMORY:
#ifdef USE_SUBEXP_CALL
    if (IS_ENCLOSE_CALLED(node)) {
      len = SIZE_OP_MEMORY_START_PUSH + tlen
	  + SIZE_OP_CALL + SIZE_OP_JUMP + SIZE_OP_RETURN;
      if (BIT_STATUS_AT(reg->bt_mem_end, node->regnum))
	len += (IS_ENCLOSE_RECURSION(node)
		? SIZE_OP_MEMORY_END_PUSH_REC : SIZE_OP_MEMORY_END_PUSH);
      else
	len += (IS_ENCLOSE_RECURSION(node)
		? SIZE_OP_MEMORY_END_REC : SIZE_OP_MEMORY_END);
    }
    else
#endif
    {
      if (BIT_STATUS_AT(reg->bt_mem_start, node->regnum))
	len = SIZE_OP_MEMORY_START_PUSH;
      else
	len = SIZE_OP_MEMORY_START;

      len += tlen + (BIT_STATUS_AT(reg->bt_mem_end, node->regnum)
		     ? SIZE_OP_MEMORY_END_PUSH : SIZE_OP_MEMORY_END);
    }
    break;

  case ENCLOSE_STOP_BACKTRACK:
    if (IS_ENCLOSE_STOP_BT_SIMPLE_REPEAT(node)) {
      QtfrNode* qn = NQTFR(node->target);
      tlen = compile_length_tree(qn->target, reg);
      if (tlen < 0) return tlen;

      len = tlen * qn->lower
	  + SIZE_OP_PUSH + tlen + SIZE_OP_POP + SIZE_OP_JUMP;
    }
    else {
      len = SIZE_OP_PUSH_STOP_BT + tlen + SIZE_OP_POP_STOP_BT;
    }
    break;

  default:
    return ONIGERR_TYPE_BUG;
    break;
  }

  return len;
}