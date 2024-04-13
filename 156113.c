string_append_listele(gstring * list, uschar sep, const uschar * ele)
{
uschar * sp;

if (list && list->ptr)
  list = string_catn(list, &sep, 1);

while((sp = Ustrchr(ele, sep)))
  {
  list = string_catn(list, ele, sp-ele+1);
  list = string_catn(list, &sep, 1);
  ele = sp+1;
  }
list = string_cat(list, ele);
(void) string_from_gstring(list);
return list;
}