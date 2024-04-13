int mysql_st_free_result_sets (SV * sth, imp_sth_t * imp_sth)
{
  dTHX;
  D_imp_dbh_from_sth;
  D_imp_xxh(sth);
  int next_result_rc= -1;

  if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
    PerlIO_printf(DBIc_LOGPIO(imp_xxh), "\t>- dbd_st_free_result_sets\n");

#if MYSQL_VERSION_ID >= MULTIPLE_RESULT_SET_VERSION
  do
  {
    if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
      PerlIO_printf(DBIc_LOGPIO(imp_xxh), "\t<- dbd_st_free_result_sets RC %d\n", next_result_rc);

    if (next_result_rc == 0)
    {
      if (!(imp_sth->result = mysql_use_result(imp_dbh->pmysql)))
      {
        /* Check for possible error */
        if (mysql_field_count(imp_dbh->pmysql))
        {
          if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
          PerlIO_printf(DBIc_LOGPIO(imp_xxh), "\t<- dbd_st_free_result_sets ERROR: %s\n",
                                  mysql_error(imp_dbh->pmysql));

          do_error(sth, mysql_errno(imp_dbh->pmysql), mysql_error(imp_dbh->pmysql),
                   mysql_sqlstate(imp_dbh->pmysql));
          return 0;
        }
      }
    }
    if (imp_sth->result)
    {
      mysql_free_result(imp_sth->result);
      imp_sth->result=NULL;
    }
  } while ((next_result_rc=mysql_next_result(imp_dbh->pmysql))==0);

  if (next_result_rc > 0)
  {
    if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
      PerlIO_printf(DBIc_LOGPIO(imp_xxh), "\t<- dbd_st_free_result_sets: Error while processing multi-result set: %s\n",
                    mysql_error(imp_dbh->pmysql));

    do_error(sth, mysql_errno(imp_dbh->pmysql), mysql_error(imp_dbh->pmysql),
             mysql_sqlstate(imp_dbh->pmysql));
  }

#else

  if (imp_sth->result)
  {
    mysql_free_result(imp_sth->result);
    imp_sth->result=NULL;
  }
#endif

  if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
    PerlIO_printf(DBIc_LOGPIO(imp_xxh), "\t<- dbd_st_free_result_sets\n");

  return 1;
}