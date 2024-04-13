string_catn(gstring * g, const uschar *s, int count)
{
int p;

if (!g)
  {
  unsigned inc = count < 4096 ? 127 : 1023;
  unsigned size = ((count + inc) &  ~inc) + 1;
  g = string_get(size);
  }

p = g->ptr;
if (p + count >= g->size)
  gstring_grow(g, p, count);

/* Because we always specify the exact number of characters to copy, we can
use memcpy(), which is likely to be more efficient than strncopy() because the
latter has to check for zero bytes. */

memcpy(g->s + p, s, count);
g->ptr = p + count;
return g;
}