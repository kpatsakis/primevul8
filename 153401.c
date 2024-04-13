fetch_char_property_to_ctype(UChar** src, UChar* end, ScanEnv* env)
{
  int r;
  OnigCodePoint c;
  OnigEncoding enc;
  UChar *prev, *start, *p;

  p = *src;
  enc = env->enc;
  r = ONIGERR_END_PATTERN_WITH_UNMATCHED_PARENTHESIS;
  start = prev = p;

  while (!PEND) {
    prev = p;
    PFETCH_S(c);
    if (c == '}') {
      r = ONIGENC_PROPERTY_NAME_TO_CTYPE(enc, start, prev);
      if (r >= 0) {
        *src = p;
      }
      else {
        onig_scan_env_set_error_string(env, r, *src, prev);
      }

      return r;
    }
    else if (c == '(' || c == ')' || c == '{' || c == '|') {
      break;
    }
  }

  return r;
}