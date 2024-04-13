set_process_info(const char *format, ...)
{
gstring gs = { .size = PROCESS_INFO_SIZE - 2, .ptr = 0, .s = process_info };
gstring * g;
int len;
va_list ap;

g = string_fmt_append(&gs, "%5d ", (int)getpid());
len = g->ptr;
va_start(ap, format);
if (!string_vformat(g, FALSE, format, ap))
  {
  gs.ptr = len;
  g = string_cat(&gs, US"**** string overflowed buffer ****");
  }
g = string_catn(g, US"\n", 1);
string_from_gstring(g);
process_info_len = g->ptr;
DEBUG(D_process_info) debug_printf("set_process_info: %s", process_info);
va_end(ap);
}