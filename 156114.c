string_append_listele_n(gstring * list, uschar sep, const uschar * ele,
 unsigned len)
{
const uschar * sp;

if (list && list->ptr)
  list = string_catn(list, &sep, 1);

while((sp = Ustrnchr(ele, sep, &len)))
  {
  list = string_catn(list, ele, sp-ele+1);
  list = string_catn(list, &sep, 1);
  ele = sp+1;
  len--;
  }
list = string_catn(list, ele, len);
(void) string_from_gstring(list);
return list;
}