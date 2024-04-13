alt_merge_opt_anc_info(OptAncInfo* to, OptAncInfo* add)
{
  to->left_anchor  &= add->left_anchor;
  to->right_anchor &= add->right_anchor;
}