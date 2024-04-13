regex_match_and_setup(const pcre *re, const uschar *subject, int options, int setup)
{
int ovector[3*(EXPAND_MAXN+1)];
uschar * s = string_copy(subject);	/* de-constifying */
int n = pcre_exec(re, NULL, CS s, Ustrlen(s), 0,
  PCRE_EOPT | options, ovector, nelem(ovector));
BOOL yield = n >= 0;
if (n == 0) n = EXPAND_MAXN + 1;
if (yield)
  {
  int nn;
  expand_nmax = setup < 0 ? 0 : setup + 1;
  for (nn = setup < 0 ? 0 : 2; nn < n*2; nn += 2)
    {
    expand_nstring[expand_nmax] = s + ovector[nn];
    expand_nlength[expand_nmax++] = ovector[nn+1] - ovector[nn];
    }
  expand_nmax--;
  }
return yield;
}