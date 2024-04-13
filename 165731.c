dbd_st_FETCH_internal(
  SV *sth,
  int what,
  MYSQL_RES *res,
  int cacheit
)
{
  dTHX;
  D_imp_sth(sth);
  AV *av= Nullav;
  MYSQL_FIELD *curField;

  /* Are we asking for a legal value? */
  if (what < 0 ||  what >= AV_ATTRIB_LAST)
    do_error(sth, JW_ERR_NOT_IMPLEMENTED, "Not implemented", NULL);

  /* Return cached value, if possible */
  else if (cacheit  &&  imp_sth->av_attr[what])
    av= imp_sth->av_attr[what];

  /* Does this sth really have a result? */
  else if (!res)
    do_error(sth, JW_ERR_NOT_ACTIVE,
	     "statement contains no result" ,NULL);
  /* Do the real work. */
  else
  {
    av= newAV();
    mysql_field_seek(res, 0);
    while ((curField= mysql_fetch_field(res)))
    {
      SV *sv;

      switch(what) {
      case AV_ATTRIB_NAME:
        sv= newSVpvn(curField->name, strlen(curField->name));
        break;

      case AV_ATTRIB_TABLE:
        sv= newSVpvn(curField->table, strlen(curField->table));
        break;

      case AV_ATTRIB_TYPE:
        sv= newSViv((int) curField->type);
        break;

      case AV_ATTRIB_SQL_TYPE:
        sv= newSViv((int) native2sql(curField->type)->data_type);
        break;
      case AV_ATTRIB_IS_PRI_KEY:
        sv= boolSV(IS_PRI_KEY(curField->flags));
        break;

      case AV_ATTRIB_IS_NOT_NULL:
        sv= boolSV(IS_NOT_NULL(curField->flags));
        break;

      case AV_ATTRIB_NULLABLE:
        sv= boolSV(!IS_NOT_NULL(curField->flags));
        break;

      case AV_ATTRIB_LENGTH:
        sv= newSViv((int) curField->length);
        break;

      case AV_ATTRIB_IS_NUM:
        sv= newSViv((int) native2sql(curField->type)->is_num);
        break;

      case AV_ATTRIB_TYPE_NAME:
        sv= newSVpv((char*) native2sql(curField->type)->type_name, 0);
        break;

      case AV_ATTRIB_MAX_LENGTH:
        sv= newSViv((int) curField->max_length);
        break;

      case AV_ATTRIB_IS_AUTO_INCREMENT:
#if defined(AUTO_INCREMENT_FLAG)
        sv= boolSV(IS_AUTO_INCREMENT(curField->flags));
        break;
#else
        croak("AUTO_INCREMENT_FLAG is not supported on this machine");
#endif

      case AV_ATTRIB_IS_KEY:
        sv= boolSV(IS_KEY(curField->flags));
        break;

      case AV_ATTRIB_IS_BLOB:
        sv= boolSV(IS_BLOB(curField->flags));
        break;

      case AV_ATTRIB_SCALE:
        sv= newSViv((int) curField->decimals);
        break;

      case AV_ATTRIB_PRECISION:
        sv= newSViv((int) (curField->length > curField->max_length) ?
                     curField->length : curField->max_length);
        break;

      default:
        sv= &PL_sv_undef;
        break;
      }
      av_push(av, sv);
    }

    /* Ensure that this value is kept, decremented in
     *  dbd_st_destroy and dbd_st_execute.  */
    if (!cacheit)
      return sv_2mortal(newRV_noinc((SV*)av));
    imp_sth->av_attr[what]= av;
  }

  if (av == Nullav)
    return &PL_sv_undef;

  return sv_2mortal(newRV_inc((SV*)av));
}