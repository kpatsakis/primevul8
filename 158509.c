header_remove(int occ, const uschar *name)
{
header_line *h;
int hcount = 0;
int len = Ustrlen(name);
for (h = header_list; h != NULL; h = h->next)
  {
  if (header_testname(h, name, len, TRUE) && (occ <= 0 || ++hcount == occ))
    {
    h->type = htype_old;
    if (occ > 0) return;
    }
  }
}