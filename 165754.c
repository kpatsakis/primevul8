AV *dbd_db_type_info_all(SV *dbh, imp_dbh_t *imp_dbh)
{
  dTHX;
  AV *av= newAV();
  AV *row;
  HV *hv;
  SV *sv;
  int i;
  const char *cols[] = {
    "TYPE_NAME",
    "DATA_TYPE",
    "COLUMN_SIZE",
    "LITERAL_PREFIX",
    "LITERAL_SUFFIX",
    "CREATE_PARAMS",
    "NULLABLE",
    "CASE_SENSITIVE",
    "SEARCHABLE",
    "UNSIGNED_ATTRIBUTE",
    "FIXED_PREC_SCALE",
    "AUTO_UNIQUE_VALUE",
    "LOCAL_TYPE_NAME",
    "MINIMUM_SCALE",
    "MAXIMUM_SCALE",
    "NUM_PREC_RADIX",
    "SQL_DATATYPE",
    "SQL_DATETIME_SUB",
    "INTERVAL_PRECISION",
    "mysql_native_type",
    "mysql_is_num"
  };

  dbh= dbh;
  imp_dbh= imp_dbh;
 
  hv= newHV();
  av_push(av, newRV_noinc((SV*) hv));
  for (i= 0;  i < (int)(sizeof(cols) / sizeof(const char*));  i++)
  {
    if (!hv_store(hv, (char*) cols[i], strlen(cols[i]), newSViv(i), 0))
    {
      SvREFCNT_dec((SV*) av);
      return Nullav;
    }
  }
  for (i= 0;  i < (int)SQL_GET_TYPE_INFO_num;  i++)
  {
    const sql_type_info_t *t= &SQL_GET_TYPE_INFO_values[i];

    row= newAV();
    av_push(av, newRV_noinc((SV*) row));
    PV_PUSH(t->type_name);
    IV_PUSH(t->data_type);
    IV_PUSH(t->column_size);
    PV_PUSH(t->literal_prefix);
    PV_PUSH(t->literal_suffix);
    PV_PUSH(t->create_params);
    IV_PUSH(t->nullable);
    IV_PUSH(t->case_sensitive);
    IV_PUSH(t->searchable);
    IV_PUSH(t->unsigned_attribute);
    IV_PUSH(t->fixed_prec_scale);
    IV_PUSH(t->auto_unique_value);
    PV_PUSH(t->local_type_name);
    IV_PUSH(t->minimum_scale);
    IV_PUSH(t->maximum_scale);

    if (t->num_prec_radix)
    {
      IV_PUSH(t->num_prec_radix);
    }
    else
      av_push(row, &PL_sv_undef);

    IV_PUSH(t->sql_datatype); /* SQL_DATATYPE*/
    IV_PUSH(t->sql_datetime_sub); /* SQL_DATETIME_SUB*/
    IV_PUSH(t->interval_precision); /* INTERVAL_PERCISION */
    IV_PUSH(t->native_type);
    IV_PUSH(t->is_num);
  }
  return av;
}