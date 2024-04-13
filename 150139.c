onig_compile(regex_t* reg, const UChar* pattern, const UChar* pattern_end,
	     OnigErrorInfo* einfo)
{
  return onig_compile_ruby(reg, pattern, pattern_end, einfo, NULL, 0);
}