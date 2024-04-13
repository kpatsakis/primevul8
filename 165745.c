SV *mysql_db_last_insert_id(SV *dbh, imp_dbh_t *imp_dbh,
        SV *catalog, SV *schema, SV *table, SV *field, SV *attr)
{
  dTHX;
  /* all these non-op settings are to stifle OS X compile warnings */
  imp_dbh= imp_dbh;
  dbh= dbh;
  catalog= catalog;
  schema= schema;
  table= table;
  field= field;
  attr= attr;

  ASYNC_CHECK_RETURN(dbh, &PL_sv_undef);
  return sv_2mortal(my_ulonglong2str(aTHX_ mysql_insert_id(imp_dbh->pmysql)));
}