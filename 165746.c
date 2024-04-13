dbd_db_rollback(SV* dbh, imp_dbh_t* imp_dbh) {
  /* croak, if not in AutoCommit mode */
  if (DBIc_has(imp_dbh, DBIcf_AutoCommit))
    return FALSE;

  ASYNC_CHECK_RETURN(dbh, FALSE);

  if (imp_dbh->has_transactions)
  {
#if MYSQL_VERSION_ID < SERVER_PREPARE_VERSION
    if (mysql_real_query(imp_dbh->pmysql, "ROLLBACK", 8))
#else
      if (mysql_rollback(imp_dbh->pmysql))
#endif
      {
        do_error(dbh, mysql_errno(imp_dbh->pmysql),
                 mysql_error(imp_dbh->pmysql) ,mysql_sqlstate(imp_dbh->pmysql));
        return FALSE;
      }
  }
  else
    do_error(dbh, JW_ERR_NOT_IMPLEMENTED,
             "Rollback ineffective because transactions are not available" ,NULL);
  return TRUE;
}