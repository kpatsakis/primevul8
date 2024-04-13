enum_func_status
php_mysqlnd_rowp_read_binary_protocol(MYSQLND_ROW_BUFFER * row_buffer, zval * fields,
									  const unsigned int field_count, const MYSQLND_FIELD * const fields_metadata,
									  const zend_bool as_int_or_float, MYSQLND_STATS * const stats)
{
	unsigned int i;
	const zend_uchar * p = row_buffer->ptr;
	const zend_uchar * null_ptr;
	zend_uchar bit;
	zval *current_field, *end_field, *start_field;

	DBG_ENTER("php_mysqlnd_rowp_read_binary_protocol");

	if (!fields) {
		DBG_RETURN(FAIL);
	}

	end_field = (start_field = fields) + field_count;

	/* skip the first byte, not EODATA_MARKER -> 0x0, status */
	p++;
	null_ptr= p;
	p += (field_count + 9)/8;	/* skip null bits */
	bit	= 4;					/* first 2 bits are reserved */

	for (i = 0, current_field = start_field; current_field < end_field; current_field++, i++) {
		enum_mysqlnd_collected_stats statistic;
		const zend_uchar * orig_p = p;

		DBG_INF_FMT("Into zval=%p decoding column %u [%s.%s.%s] type=%u field->flags&unsigned=%u flags=%u is_bit=%u",
			current_field, i,
			fields_metadata[i].db, fields_metadata[i].table, fields_metadata[i].name, fields_metadata[i].type,
			fields_metadata[i].flags & UNSIGNED_FLAG, fields_metadata[i].flags, fields_metadata[i].type == MYSQL_TYPE_BIT);
		if (*null_ptr & bit) {
			DBG_INF("It's null");
			ZVAL_NULL(current_field);
			statistic = STAT_BINARY_TYPE_FETCHED_NULL;
		} else {
			enum_mysqlnd_field_types type = fields_metadata[i].type;
			mysqlnd_ps_fetch_functions[type].func(current_field, &fields_metadata[i], 0, &p);

			if (MYSQLND_G(collect_statistics)) {
				switch (fields_metadata[i].type) {
					case MYSQL_TYPE_DECIMAL:	statistic = STAT_BINARY_TYPE_FETCHED_DECIMAL; break;
					case MYSQL_TYPE_TINY:		statistic = STAT_BINARY_TYPE_FETCHED_INT8; break;
					case MYSQL_TYPE_SHORT:		statistic = STAT_BINARY_TYPE_FETCHED_INT16; break;
					case MYSQL_TYPE_LONG:		statistic = STAT_BINARY_TYPE_FETCHED_INT32; break;
					case MYSQL_TYPE_FLOAT:		statistic = STAT_BINARY_TYPE_FETCHED_FLOAT; break;
					case MYSQL_TYPE_DOUBLE:		statistic = STAT_BINARY_TYPE_FETCHED_DOUBLE; break;
					case MYSQL_TYPE_NULL:		statistic = STAT_BINARY_TYPE_FETCHED_NULL; break;
					case MYSQL_TYPE_TIMESTAMP:	statistic = STAT_BINARY_TYPE_FETCHED_TIMESTAMP; break;
					case MYSQL_TYPE_LONGLONG:	statistic = STAT_BINARY_TYPE_FETCHED_INT64; break;
					case MYSQL_TYPE_INT24:		statistic = STAT_BINARY_TYPE_FETCHED_INT24; break;
					case MYSQL_TYPE_DATE:		statistic = STAT_BINARY_TYPE_FETCHED_DATE; break;
					case MYSQL_TYPE_TIME:		statistic = STAT_BINARY_TYPE_FETCHED_TIME; break;
					case MYSQL_TYPE_DATETIME:	statistic = STAT_BINARY_TYPE_FETCHED_DATETIME; break;
					case MYSQL_TYPE_YEAR:		statistic = STAT_BINARY_TYPE_FETCHED_YEAR; break;
					case MYSQL_TYPE_NEWDATE:	statistic = STAT_BINARY_TYPE_FETCHED_DATE; break;
					case MYSQL_TYPE_VARCHAR:	statistic = STAT_BINARY_TYPE_FETCHED_STRING; break;
					case MYSQL_TYPE_BIT:		statistic = STAT_BINARY_TYPE_FETCHED_BIT; break;
					case MYSQL_TYPE_NEWDECIMAL:	statistic = STAT_BINARY_TYPE_FETCHED_DECIMAL; break;
					case MYSQL_TYPE_ENUM:		statistic = STAT_BINARY_TYPE_FETCHED_ENUM; break;
					case MYSQL_TYPE_SET:		statistic = STAT_BINARY_TYPE_FETCHED_SET; break;
					case MYSQL_TYPE_TINY_BLOB:	statistic = STAT_BINARY_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_MEDIUM_BLOB:statistic = STAT_BINARY_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_LONG_BLOB:	statistic = STAT_BINARY_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_BLOB:		statistic = STAT_BINARY_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_VAR_STRING:	statistic = STAT_BINARY_TYPE_FETCHED_STRING; break;
					case MYSQL_TYPE_STRING:		statistic = STAT_BINARY_TYPE_FETCHED_STRING; break;
					case MYSQL_TYPE_GEOMETRY:	statistic = STAT_BINARY_TYPE_FETCHED_GEOMETRY; break;
					default: statistic = STAT_BINARY_TYPE_FETCHED_OTHER; break;
				}
			}
		}
		MYSQLND_INC_CONN_STATISTIC_W_VALUE2(stats, statistic, 1,
										STAT_BYTES_RECEIVED_PURE_DATA_PS,
										(Z_TYPE_P(current_field) == IS_STRING)?
											Z_STRLEN_P(current_field) : (size_t)(p - orig_p));

		if (!((bit<<=1) & 255)) {
			bit = 1;	/* to the following byte */
			null_ptr++;
		}
	}

	DBG_RETURN(PASS);