static char *safe_hv_fetch(pTHX_ HV *hv, const char *name, int name_length)
{
  SV** svp;
  STRLEN len;
  char *res= NULL;

  if ((svp= hv_fetch(hv, name, name_length, FALSE)))
  {
    res= SvPV(*svp, len);
    if (!len)
      res= NULL;
  }
  return res;
}