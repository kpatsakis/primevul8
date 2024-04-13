static imp_sth_ph_t *alloc_param(int num_params)
{
  imp_sth_ph_t *params;

  if (num_params)
    Newz(908, params, (unsigned int) num_params, imp_sth_ph_t);
  else
    params= NULL;

  return params;
}