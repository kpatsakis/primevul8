node_new_anychar_with_fixed_option(OnigOptionType option)
{
  CtypeNode* ct;
  Node* node;

  node = node_new_anychar();
  CHECK_NULL_RETURN(node);

  ct = CTYPE_(node);
  ct->options = option;
  NODE_STATUS_ADD(node, FIXED_OPTION);
  return node;
}