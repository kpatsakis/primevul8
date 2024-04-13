onig_st_init_callout_name_table_with_size(int size)
{
  static struct st_hash_type hashType = {
    callout_name_table_cmp,
    callout_name_table_hash,
  };

  return (hash_table_type* )
           onig_st_init_table_with_size(&hashType, size);
}