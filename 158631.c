regex_must_compile(const uschar *pattern, BOOL caseless, BOOL use_malloc)
{
int offset;
int options = PCRE_COPT;
const pcre *yield;
const uschar *error;
if (use_malloc)
  {
  pcre_malloc = function_store_malloc;
  pcre_free = function_store_free;
  }
if (caseless) options |= PCRE_CASELESS;
yield = pcre_compile(CCS pattern, options, (const char **)&error, &offset, NULL);
pcre_malloc = function_store_get;
pcre_free = function_dummy_free;
if (yield == NULL)
  log_write(0, LOG_MAIN|LOG_PANIC_DIE, "regular expression error: "
    "%s at offset %d while compiling %s", error, offset, pattern);
return yield;
}