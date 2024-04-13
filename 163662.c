strqueue_sort_cmp(const void *ap, const void *bp, void *dp)
{
  const char *a = *(const char **)ap;
  const char *b = *(const char **)bp;
  return strcmp(a ? a : "", b ? b : "");
}