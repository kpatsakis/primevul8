dbd_st_STORE_attrib(
                    SV *sth,
                    imp_sth_t *imp_sth,
                    SV *keysv,
                    SV *valuesv
                   )
{
  dTHX;
  STRLEN(kl);
  char *key= SvPV(keysv, kl);
  int retval= FALSE;
  D_imp_xxh(sth);

  if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
    PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                  "\t\t-> dbd_st_STORE_attrib for %p, key %s\n",
                  sth, key);

  if (strEQ(key, "mysql_use_result"))
  {
    imp_sth->use_mysql_use_result= SvTRUE(valuesv);
  }

  if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
    PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                  "\t\t<- dbd_st_STORE_attrib for %p, result %d\n",
                  sth, retval);

  return retval;
}