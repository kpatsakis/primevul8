void dbd_st_destroy(SV *sth, imp_sth_t *imp_sth) {
  dTHX;
  D_imp_xxh(sth);

#if defined (dTHR)
  dTHR;
#endif

  int i;

#if MYSQL_VERSION_ID >= SERVER_PREPARE_VERSION
  imp_sth_fbh_t *fbh;
  int n;

  n= DBIc_NUM_PARAMS(imp_sth);
  if (n)
  {
    if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
      PerlIO_printf(DBIc_LOGPIO(imp_xxh), "\tFreeing %d parameters, bind %p fbind %p\n",
          n, imp_sth->bind, imp_sth->fbind);

    free_bind(imp_sth->bind);
    free_fbind(imp_sth->fbind);
  }

  fbh= imp_sth->fbh;
  if (fbh)
  {
    n = DBIc_NUM_FIELDS(imp_sth);
    i = 0;
    while (i < n)
    {
      if (fbh[i].data) Safefree(fbh[i].data);
      ++i;
    }

    free_fbuffer(fbh);
    if (imp_sth->buffer)
      free_bind(imp_sth->buffer);
  }

  if (imp_sth->stmt)
  {
    if (mysql_stmt_close(imp_sth->stmt))
    {
      do_error(DBIc_PARENT_H(imp_sth), mysql_stmt_errno(imp_sth->stmt),
          mysql_stmt_error(imp_sth->stmt),
          mysql_stmt_sqlstate(imp_sth->stmt));
    }
  }
#endif


  /* dbd_st_finish has already been called by .xs code if needed.	*/

  /* Free values allocated by dbd_bind_ph */
  if (imp_sth->params)
  {
    free_param(aTHX_ imp_sth->params, DBIc_NUM_PARAMS(imp_sth));
    imp_sth->params= NULL;
  }

  /* Free cached array attributes */
  for (i= 0; i < AV_ATTRIB_LAST; i++)
  {
    if (imp_sth->av_attr[i])
      SvREFCNT_dec(imp_sth->av_attr[i]);
    imp_sth->av_attr[i]= Nullav;
  }
  /* let DBI know we've done it   */
  DBIc_IMPSET_off(imp_sth);
}