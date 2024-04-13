node_new_option(OnigOptionType option)
{
  Node* node = node_new_bag(BAG_OPTION);
  CHECK_NULL_RETURN(node);
  BAG_(node)->o.options = option;
  return node;
}