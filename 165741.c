static imp_sth_phb_t *alloc_fbind(int num_params)
{
  imp_sth_phb_t *fbind;

  if (num_params)
    Newz(908, fbind, (unsigned int) num_params, imp_sth_phb_t);
  else
    fbind= NULL;

  return fbind;
}