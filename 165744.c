int bind_param(imp_sth_ph_t *ph, SV *value, IV sql_type)
{
  dTHX;
  if (ph->value)
  {
    if (SvMAGICAL(ph->value))
      mg_get(ph->value);
    (void) SvREFCNT_dec(ph->value);
  }

  ph->value= newSVsv(value);

  if (sql_type)
    ph->type = sql_type;

  return TRUE;
}