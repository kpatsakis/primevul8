strcat_capa_from_static(UChar* dest, UChar* dest_end,
                        const UChar* src, const UChar* src_end, int capa)
{
  UChar* r;

  r = (UChar* )xmalloc(capa + 1);
  CHECK_NULL_RETURN(r);
  onig_strcpy(r, dest, dest_end);
  onig_strcpy(r + (dest_end - dest), src, src_end);
  return r;
}