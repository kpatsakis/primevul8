fn_hdrs_added(void)
{
gstring * g = NULL;
header_line * h;

for (h = acl_added_headers; h; h = h->next)
  {
  int i = h->slen;
  if (h->text[i-1] == '\n') i--;
  g = string_append_listele_n(g, '\n', h->text, i);
  }

return g ? g->s : NULL;
}