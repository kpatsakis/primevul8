is_code_ctype(OnigCodePoint code, unsigned int ctype, OnigEncoding enc ARG_UNUSED)
{
  if (code < 256)
    return ENC_IS_CP1253_CTYPE(code, ctype);
  else
    return FALSE;
}