clear_opt_exact_info(OptExactInfo* ex)
{
  clear_mml(&ex->mmd);
  clear_opt_anc_info(&ex->anc);
  ex->reach_end   = 0;
  ex->ignore_case = 0;
  ex->len         = 0;
  ex->s[0]        = '\0';
}