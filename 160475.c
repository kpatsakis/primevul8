id2hr_t *lookup_id2hr(unsigned long long id, id2hr_t *table, ushort nEntries)
{
  for (int k = 0; k < nEntries; k++)
    if (id == table[k].id)
      return &table[k];
  return 0;
}