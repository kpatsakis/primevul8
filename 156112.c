string_append2_listele_n(gstring * list, const uschar * sepstr,
 const uschar * ele, unsigned len)
{
if (list && list->ptr)
  list = string_cat(list, sepstr);

list = string_catn(list, ele, len);
(void) string_from_gstring(list);
return list;
}