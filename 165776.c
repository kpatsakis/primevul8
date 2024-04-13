int mysql_st_clean_cursor(SV* sth, imp_sth_t* imp_sth) {

  if (DBIc_ACTIVE(imp_sth) && dbd_describe(sth, imp_sth) &&
      !imp_sth->fetch_done)
    mysql_stmt_free_result(imp_sth->stmt);
  return 1;
}