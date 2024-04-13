history_tree_free(OnigCaptureTreeNode* node)
{
  history_tree_clear(node);
  if (IS_NOT_NULL(node->childs)) xfree(node->childs);

  xfree(node);
}