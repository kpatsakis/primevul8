static imp_sth_fbh_t *alloc_fbuffer(int num_fields)
{
  imp_sth_fbh_t *fbh;

  if (num_fields)
    Newz(908, fbh, (unsigned int) num_fields, imp_sth_fbh_t);
  else
    fbh= NULL;

  return fbh;
}