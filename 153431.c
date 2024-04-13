st_insert_callout_name_table(hash_table_type* table,
                             OnigEncoding enc, int type,
                             UChar* str_key, UChar* end_key,
                             hash_data_type value)
{
  st_callout_name_key* key;
  int result;

  key = (st_callout_name_key* )xmalloc(sizeof(st_callout_name_key));
  CHECK_NULL_RETURN_MEMERR(key);

  /* key->s: don't duplicate, because str_key is duped in callout_name_entry() */
  key->enc  = enc;
  key->type = type;
  key->s    = str_key;
  key->end  = end_key;
  result = onig_st_insert(table, (st_data_t )key, value);
  if (result) {
    xfree(key);
  }
  return result;
}