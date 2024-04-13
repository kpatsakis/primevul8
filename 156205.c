sunday_quick_search(regex_t* reg, const UChar* target, const UChar* target_end,
                    const UChar* text, const UChar* text_end,
                    const UChar* text_range)
{
  const UChar *s, *t, *p, *end;
  const UChar *tail;
  int map_offset;

  end = text_range + (target_end - target);
  if (end > text_end)
    end = text_end;

  map_offset = reg->map_offset;
  tail = target_end - 1;
  s = text + (tail - target);

  while (s < end) {
    p = s;
    t = tail;
    while (*p == *t) {
      if (t == target) return (UChar* )p;
      p--; t--;
    }
    if (s + map_offset >= text_end) break;
    s += reg->map[*(s + map_offset)];
  }

  return (UChar* )NULL;
}