history_root_free(OnigRegion* r)
{
  if (IS_NULL(r->history_root)) return ;

  history_tree_free(r->history_root);
  r->history_root = (OnigCaptureTreeNode* )0;
}