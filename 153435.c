onig_st_insert_strend(hash_table_type* table, const UChar* str_key,
                      const UChar* end_key, hash_data_type value)
{
  st_str_end_key* key;
  int result;

  key = (st_str_end_key* )xmalloc(sizeof(st_str_end_key));
  CHECK_NULL_RETURN_MEMERR(key);

  key->s   = (UChar* )str_key;
  key->end = (UChar* )end_key;
  result = onig_st_insert(table, (st_data_t )key, value);
  if (result) {
    xfree(key);
  }
  return result;
}