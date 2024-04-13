onigenc_mb4_is_code_ctype(OnigEncoding enc, OnigCodePoint code,
			  unsigned int ctype)
{
  if (code < 128)
    return ONIGENC_IS_ASCII_CODE_CTYPE(code, ctype);
  else {
    if (CTYPE_IS_WORD_GRAPH_PRINT(ctype)) {
      return (ONIGENC_CODE_TO_MBCLEN(enc, code) > 1 ? TRUE : FALSE);
    }
  }

  return FALSE;
}