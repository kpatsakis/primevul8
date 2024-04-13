slow_search_ic(OnigEncoding enc, int case_fold_flag,
               UChar* target, UChar* target_end,
               const UChar* text, const UChar* text_end, UChar* text_range)
{
  UChar *s;

  s = (UChar* )text;

  while (s < text_range) {
    if (str_lower_case_match(enc, case_fold_flag, target, target_end,
                             s, text_end))
      return s;

    s += enclen(enc, s);
  }

  return (UChar* )NULL;
}