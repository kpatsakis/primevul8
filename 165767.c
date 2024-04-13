int mysql_db_async_ready(SV* h)
{
  dTHX;
  D_imp_xxh(h);
  imp_dbh_t* dbh;
  int htype;

  htype = DBIc_TYPE(imp_xxh);
  
  if(htype == DBIt_DB) {
      D_imp_dbh(h);
      dbh = imp_dbh;
  } else {
      D_imp_sth(h);
      D_imp_dbh_from_sth;
      dbh = imp_dbh;
  }

  if(dbh->async_query_in_flight) {
      if(dbh->async_query_in_flight == imp_xxh) {
          struct pollfd fds;
          int retval;

          fds.fd = dbh->pmysql->net.fd;
          fds.events = POLLIN;

          retval = poll(&fds, 1, 0);

          if(retval < 0) {
              do_error(h, errno, strerror(errno), "HY000");
          }
          return retval;
      } else {
          do_error(h, 2000, "Calling mysql_async_ready on the wrong handle", "HY000");
          return -1;
      }
  } else {
      do_error(h, 2000, "Handle is not in asynchronous mode", "HY000");
      return -1;
  }
}