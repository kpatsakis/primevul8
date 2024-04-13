string_get_localpart(address_item * addr, gstring * yield)
{
uschar * s;

s = addr->prefix;
if (testflag(addr, af_include_affixes) && s)
  {
#ifdef SUPPORT_I18N
  if (testflag(addr, af_utf8_downcvt))
    s = string_localpart_utf8_to_alabel(s, NULL);
#endif
  yield = string_cat(yield, s);
  }

s = addr->local_part;
#ifdef SUPPORT_I18N
if (testflag(addr, af_utf8_downcvt))
  s = string_localpart_utf8_to_alabel(s, NULL);
#endif
yield = string_cat(yield, s);

s = addr->suffix;
if (testflag(addr, af_include_affixes) && s)
  {
#ifdef SUPPORT_I18N
  if (testflag(addr, af_utf8_downcvt))
    s = string_localpart_utf8_to_alabel(s, NULL);
#endif
  yield = string_cat(yield, s);
  }

return yield;
}