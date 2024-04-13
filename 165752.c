int dbd_st_finish(SV* sth, imp_sth_t* imp_sth) {
  dTHX;
  D_imp_xxh(sth);

#if defined (dTHR)
  dTHR;
#endif

#if MYSQL_ASYNC
  D_imp_dbh_from_sth;
  if(imp_dbh->async_query_in_flight) {
    mysql_db_async_result(sth, &imp_sth->result);
  }
#endif

#if MYSQL_VERSION_ID >= SERVER_PREPARE_VERSION
  if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
  {
    PerlIO_printf(DBIc_LOGPIO(imp_xxh), "\n--> dbd_st_finish\n");
  }

  if (imp_sth->use_server_side_prepare)
  {
    if (imp_sth && imp_sth->stmt)
    {
      if (!mysql_st_clean_cursor(sth, imp_sth))
      {
        do_error(sth, JW_ERR_SEQUENCE,
                 "Error happened while tried to clean up stmt",NULL);
        return 0;
      }
    }
  }
#endif

  /*
    Cancel further fetches from this cursor.
    We don't close the cursor till DESTROY.
    The application may re execute it.
  */
  if (imp_sth && DBIc_ACTIVE(imp_sth))
  {
    /*
      Clean-up previous result set(s) for sth to prevent
      'Commands out of sync' error
    */
    mysql_st_free_result_sets(sth, imp_sth);
  }
  DBIc_ACTIVE_off(imp_sth);
  if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
  {
    PerlIO_printf(DBIc_LOGPIO(imp_xxh), "\n<-- dbd_st_finish\n");
  }
  return 1;
}