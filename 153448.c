onig_st_lookup_strend(hash_table_type* table, const UChar* str_key,
                      const UChar* end_key, hash_data_type *value)
{
  st_str_end_key key;

  key.s   = (UChar* )str_key;
  key.end = (UChar* )end_key;

  return onig_st_lookup(table, (st_data_t )(&key), value);
}