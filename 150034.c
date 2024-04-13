map_search(OnigEncoding enc, UChar map[],
	   const UChar* text, const UChar* text_range, const UChar* text_end)
{
  const UChar *s = text;

  while (s < text_range) {
    if (map[*s]) return (UChar* )s;

    s += enclen(enc, s, text_end);
  }
  return (UChar* )NULL;
}