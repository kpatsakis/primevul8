static const sql_type_info_t *native2sql(int t)
{
  switch (t) {
    case FIELD_TYPE_VAR_STRING:  return &SQL_GET_TYPE_INFO_values[0];
    case FIELD_TYPE_DECIMAL:     return &SQL_GET_TYPE_INFO_values[1];
#ifdef FIELD_TYPE_NEWDECIMAL
    case FIELD_TYPE_NEWDECIMAL:  return &SQL_GET_TYPE_INFO_values[1];
#endif
    case FIELD_TYPE_TINY:        return &SQL_GET_TYPE_INFO_values[2];
    case FIELD_TYPE_SHORT:       return &SQL_GET_TYPE_INFO_values[3];
    case FIELD_TYPE_LONG:        return &SQL_GET_TYPE_INFO_values[4];
    case FIELD_TYPE_FLOAT:       return &SQL_GET_TYPE_INFO_values[5];

    /* 6  */
    case FIELD_TYPE_DOUBLE:      return &SQL_GET_TYPE_INFO_values[7];
    case FIELD_TYPE_TIMESTAMP:   return &SQL_GET_TYPE_INFO_values[8];
    case FIELD_TYPE_LONGLONG:    return &SQL_GET_TYPE_INFO_values[9];
    case FIELD_TYPE_INT24:       return &SQL_GET_TYPE_INFO_values[10];
    case FIELD_TYPE_DATE:        return &SQL_GET_TYPE_INFO_values[11];
    case FIELD_TYPE_TIME:        return &SQL_GET_TYPE_INFO_values[12];
    case FIELD_TYPE_DATETIME:    return &SQL_GET_TYPE_INFO_values[13];
    case FIELD_TYPE_YEAR:        return &SQL_GET_TYPE_INFO_values[14];
    case FIELD_TYPE_NEWDATE:     return &SQL_GET_TYPE_INFO_values[15];
    case FIELD_TYPE_ENUM:        return &SQL_GET_TYPE_INFO_values[16];
    case FIELD_TYPE_SET:         return &SQL_GET_TYPE_INFO_values[17];
    case FIELD_TYPE_BLOB:        return &SQL_GET_TYPE_INFO_values[18];
    case FIELD_TYPE_TINY_BLOB:   return &SQL_GET_TYPE_INFO_values[19];
    case FIELD_TYPE_MEDIUM_BLOB: return &SQL_GET_TYPE_INFO_values[20];
    case FIELD_TYPE_LONG_BLOB:   return &SQL_GET_TYPE_INFO_values[21];
    case FIELD_TYPE_STRING:      return &SQL_GET_TYPE_INFO_values[22];
    default:                     return &SQL_GET_TYPE_INFO_values[0];
  }
}