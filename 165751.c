int dbd_db_login(SV* dbh, imp_dbh_t* imp_dbh, char* dbname, char* user,
		 char* password) {
#ifdef dTHR
  dTHR;
#endif
  dTHX; 
  D_imp_xxh(dbh);

  if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
    PerlIO_printf(DBIc_LOGPIO(imp_xxh),
		  "imp_dbh->connect: dsn = %s, uid = %s, pwd = %s\n",
		  dbname ? dbname : "NULL",
		  user ? user : "NULL",
		  password ? password : "NULL");

  imp_dbh->stats.auto_reconnects_ok= 0;
  imp_dbh->stats.auto_reconnects_failed= 0;
  imp_dbh->bind_type_guessing= FALSE;
  imp_dbh->bind_comment_placeholders= FALSE;
  imp_dbh->has_transactions= TRUE;
 /* Safer we flip this to TRUE perl side if we detect a mod_perl env. */
  imp_dbh->auto_reconnect = FALSE;

  /* HELMUT */
#if defined(sv_utf8_decode) && MYSQL_VERSION_ID >=SERVER_PREPARE_VERSION
  imp_dbh->enable_utf8 = FALSE;     /* initialize mysql_enable_utf8 */
  imp_dbh->enable_utf8mb4 = FALSE;  /* initialize mysql_enable_utf8mb4 */
#endif

  if (!my_login(aTHX_ dbh, imp_dbh))
  {
    if(imp_dbh->pmysql) {
        do_error(dbh, mysql_errno(imp_dbh->pmysql),
                mysql_error(imp_dbh->pmysql) ,mysql_sqlstate(imp_dbh->pmysql));
        Safefree(imp_dbh->pmysql);

    }
    return FALSE;
  }

    /*
     *  Tell DBI, that dbh->disconnect should be called for this handle
     */
    DBIc_ACTIVE_on(imp_dbh);

    /* Tell DBI, that dbh->destroy should be called for this handle */
    DBIc_on(imp_dbh, DBIcf_IMPSET);

    return TRUE;
}