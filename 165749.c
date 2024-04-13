void dbd_db_destroy(SV* dbh, imp_dbh_t* imp_dbh) {

    /*
     *  Being on the safe side never hurts ...
     */
  if (DBIc_ACTIVE(imp_dbh))
  {
    if (imp_dbh->has_transactions)
    {
      if (!DBIc_has(imp_dbh, DBIcf_AutoCommit))
#if MYSQL_VERSION_ID < SERVER_PREPARE_VERSION
        if ( mysql_real_query(imp_dbh->pmysql, "ROLLBACK", 8))
#else
        if (mysql_rollback(imp_dbh->pmysql))
#endif
            do_error(dbh, TX_ERR_ROLLBACK,"ROLLBACK failed" ,NULL);
    }
    dbd_db_disconnect(dbh, imp_dbh);
  }
  Safefree(imp_dbh->pmysql);

  /* Tell DBI, that dbh->destroy must no longer be called */
  DBIc_off(imp_dbh, DBIcf_IMPSET);
}