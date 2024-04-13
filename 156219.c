onig_init_for_match_at(regex_t* reg)
{
  return match_at(reg, (const UChar* )NULL, (const UChar* )NULL,
                  (const UChar* )NULL, (const UChar* )NULL, (UChar* )NULL,
                  (MatchArg* )NULL);
}