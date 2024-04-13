add_opt_anc_info(OptAncInfo* to, int anc)
{
  if (is_left_anchor(anc))
    to->left_anchor |= anc;
  else
    to->right_anchor |= anc;
}