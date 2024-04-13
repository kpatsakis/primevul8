sunday_quick_search_step_forward(regex_t* reg,
                                 const UChar* target, const UChar* target_end,
                                 const UChar* text, const UChar* text_end,
                                 const UChar* text_range)
{
  const UChar *s, *se, *t, *p, *end;
  const UChar *tail;
  int skip, tlen1;
  int map_offset;
  OnigEncoding enc;

#ifdef ONIG_DEBUG_SEARCH
  fprintf(stderr,
          "sunday_quick_search_step_forward: text: %p, text_end: %p, text_range: %p\n", text, text_end, text_range);
#endif

  enc = reg->enc;

  tail = target_end - 1;
  tlen1 = (int )(tail - target);
  end = text_range;
  if (end + tlen1 > text_end)
    end = text_end - tlen1;

  map_offset = reg->map_offset;
  s = text;

  while (s < end) {
    p = se = s + tlen1;
    t = tail;
    while (*p == *t) {
      if (t == target) return (UChar* )s;
      p--; t--;
    }
    if (se + map_offset >= text_end) break;
    skip = reg->map[*(se + map_offset)];
#if 0
    t = s;
    do {
      s += enclen(enc, s);
    } while ((s - t) < skip && s < end);
#else
    s += skip;
    if (s < end)
      s = onigenc_get_right_adjust_char_head(enc, text, s);
#endif
  }

  return (UChar* )NULL;
}