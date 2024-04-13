is_anychar_star_quantifier(QtfrNode* qn)
{
  if (qn->greedy && IS_REPEAT_INFINITE(qn->upper) &&
      NTYPE(qn->target) == NT_CANY)
    return 1;
  else
    return 0;
}