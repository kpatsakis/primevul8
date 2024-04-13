is_allowed_callout_tag_name(OnigEncoding enc, UChar* name, UChar* name_end)
{
  UChar* p;
  OnigCodePoint c;

  if (name >= name_end) return 0;

  p = name;
  while (p < name_end) {
    c = ONIGENC_MBC_TO_CODE(enc, p, name_end);
    if (! IS_ALLOWED_CODE_IN_CALLOUT_TAG_NAME(c))
      return 0;

    if (p == name) {
      if (c >= '0' && c <= '9') return 0;
    }

    p += ONIGENC_MBC_ENC_LEN(enc, p);
  }

  return 1;
}