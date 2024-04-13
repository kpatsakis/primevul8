strcat_capa(UChar* dest, UChar* dest_end, const UChar* src, const UChar* src_end,
            int capa)
{
  UChar* r;

  if (dest)
    r = (UChar* )xrealloc(dest, capa + 1);
  else
    r = (UChar* )xmalloc(capa + 1);

  CHECK_NULL_RETURN(r);
  onig_strcpy(r + (dest_end - dest), src, src_end);
  return r;
}