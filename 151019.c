concat_opt_anc_info(OptAncInfo* to, OptAncInfo* left, OptAncInfo* right,
		    OnigDistance left_len, OnigDistance right_len)
{
  clear_opt_anc_info(to);

  to->left_anchor = left->left_anchor;
  if (left_len == 0) {
    to->left_anchor |= right->left_anchor;
  }

  to->right_anchor = right->right_anchor;
  if (right_len == 0) {
    to->right_anchor |= left->right_anchor;
  }
}