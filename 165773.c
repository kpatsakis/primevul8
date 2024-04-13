free_param(pTHX_ imp_sth_ph_t *params, int num_params)
{
  if (params)
  {
    int i;
    for (i= 0;  i < num_params;  i++)
    {
      imp_sth_ph_t *ph= params+i;
      if (ph->value)
      {
        (void) SvREFCNT_dec(ph->value);
        ph->value= NULL;
      }
    }
    Safefree(params);
  }
}