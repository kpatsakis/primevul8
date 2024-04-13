parse_regexp(Node** top, UChar** src, UChar* end, ScanEnv* env)
{
  int r;
  PToken tok;

  r = fetch_token(&tok, src, end, env);
  if (r < 0) return r;
  r = parse_alts(top, &tok, TK_EOT, src, end, env, FALSE);
  if (r < 0) return r;

  return 0;
}