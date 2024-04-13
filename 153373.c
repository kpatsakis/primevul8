callout_name_find(OnigEncoding enc, int is_not_single,
                  const UChar* name, const UChar* name_end)
{
  int r;
  CalloutNameEntry* e;
  CalloutNameTable* t = GlobalCalloutNameTable;

  e = (CalloutNameEntry* )NULL;
  if (IS_NOT_NULL(t)) {
    r = onig_st_lookup_callout_name_table(t, enc, is_not_single, name, name_end,
                                          (HashDataType* )((void* )(&e)));
    if (r == 0) { /* not found */
      if (enc != ONIG_ENCODING_ASCII &&
          ONIGENC_IS_ASCII_COMPATIBLE_ENCODING(enc)) {
        enc = ONIG_ENCODING_ASCII;
        onig_st_lookup_callout_name_table(t, enc, is_not_single, name, name_end,
                                          (HashDataType* )((void* )(&e)));
      }
    }
  }
  return e;
}