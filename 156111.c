string_cat(gstring *string, const uschar *s)
{
return string_catn(string, s, Ustrlen(s));
}