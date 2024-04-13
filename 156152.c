slow_search_backward(OnigEncoding enc, UChar* target, UChar* target_end,
                     const UChar* text, const UChar* adjust_text,
                     const UChar* text_end, const UChar* text_start)
{
  UChar *t, *p, *s;

  s = (UChar* )text_end;
  s -= (target_end - target);
  if (s > text_start)
    s = (UChar* )text_start;
  else
    s = ONIGENC_LEFT_ADJUST_CHAR_HEAD(enc, adjust_text, s);

  while (s >= text) {
    if (*s == *target) {
      p = s + 1;
      t = target + 1;
      while (t < target_end) {
        if (*t != *p++)
          break;
        t++;
      }
      if (t == target_end)
        return s;
    }
    s = (UChar* )onigenc_get_prev_char_head(enc, adjust_text, s);
  }

  return (UChar* )NULL;
}