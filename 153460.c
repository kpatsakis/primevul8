onig_st_lookup_callout_name_table(hash_table_type* table,
                                  OnigEncoding enc,
                                  int type,
                                  const UChar* str_key,
                                  const UChar* end_key,
                                  hash_data_type *value)
{
  st_callout_name_key key;

  key.enc  = enc;
  key.type = type;
  key.s    = (UChar* )str_key;
  key.end  = (UChar* )end_key;

  return onig_st_lookup(table, (st_data_t )(&key), value);
}