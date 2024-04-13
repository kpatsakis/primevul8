compile_option_node(EncloseNode* node, regex_t* reg)
{
  int r;
  OnigOptionType prev = reg->options;

  if (IS_DYNAMIC_OPTION(prev ^ node->option)) {
    r = add_opcode_option(reg, OP_SET_OPTION_PUSH, node->option);
    if (r) return r;
    r = add_opcode_option(reg, OP_SET_OPTION, prev);
    if (r) return r;
    r = add_opcode(reg, OP_FAIL);
    if (r) return r;
  }

  reg->options = node->option;
  r = compile_tree(node->target, reg);
  reg->options = prev;

  if (IS_DYNAMIC_OPTION(prev ^ node->option)) {
    if (r) return r;
    r = add_opcode_option(reg, OP_SET_OPTION, prev);
  }
  return r;
}