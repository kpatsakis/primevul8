enum_func_status
php_mysqlnd_rowp_read_text_protocol_aux(MYSQLND_ROW_BUFFER * row_buffer, zval * fields,
									unsigned int field_count, const MYSQLND_FIELD * fields_metadata,
									zend_bool as_int_or_float, MYSQLND_STATS * stats)
{
	unsigned int i;
	zval *current_field, *end_field, *start_field;
	zend_uchar * p = row_buffer->ptr;
	const size_t data_size = row_buffer->size;
	const zend_uchar * const packet_end = (zend_uchar*) p + data_size;

	DBG_ENTER("php_mysqlnd_rowp_read_text_protocol_aux");

	if (!fields) {
		DBG_RETURN(FAIL);
	}

	end_field = (start_field = fields) + field_count;

	for (i = 0, current_field = start_field; current_field < end_field; current_field++, i++) {
		/* php_mysqlnd_net_field_length() call should be after *this_field_len_pos = p; */
		const zend_ulong len = php_mysqlnd_net_field_length((const zend_uchar **) &p);

		/* NULL or NOT NULL, this is the question! */
		if (len == MYSQLND_NULL_LENGTH) {
			ZVAL_NULL(current_field);
		} else if ((p + len) > packet_end) {
			php_error_docref(NULL, E_WARNING, "Malformed server packet. Field length pointing "MYSQLND_SZ_T_SPEC
											  " bytes after end of packet", (p + len) - packet_end - 1);
			DBG_RETURN(FAIL);
		} else {
#if defined(MYSQLND_STRING_TO_INT_CONVERSION)
			struct st_mysqlnd_perm_bind perm_bind =
					mysqlnd_ps_fetch_functions[fields_metadata[i].type];
#endif
			if (MYSQLND_G(collect_statistics)) {
				enum_mysqlnd_collected_stats statistic;
				switch (fields_metadata[i].type) {
					case MYSQL_TYPE_DECIMAL:	statistic = STAT_TEXT_TYPE_FETCHED_DECIMAL; break;
					case MYSQL_TYPE_TINY:		statistic = STAT_TEXT_TYPE_FETCHED_INT8; break;
					case MYSQL_TYPE_SHORT:		statistic = STAT_TEXT_TYPE_FETCHED_INT16; break;
					case MYSQL_TYPE_LONG:		statistic = STAT_TEXT_TYPE_FETCHED_INT32; break;
					case MYSQL_TYPE_FLOAT:		statistic = STAT_TEXT_TYPE_FETCHED_FLOAT; break;
					case MYSQL_TYPE_DOUBLE:		statistic = STAT_TEXT_TYPE_FETCHED_DOUBLE; break;
					case MYSQL_TYPE_NULL:		statistic = STAT_TEXT_TYPE_FETCHED_NULL; break;
					case MYSQL_TYPE_TIMESTAMP:	statistic = STAT_TEXT_TYPE_FETCHED_TIMESTAMP; break;
					case MYSQL_TYPE_LONGLONG:	statistic = STAT_TEXT_TYPE_FETCHED_INT64; break;
					case MYSQL_TYPE_INT24:		statistic = STAT_TEXT_TYPE_FETCHED_INT24; break;
					case MYSQL_TYPE_DATE:		statistic = STAT_TEXT_TYPE_FETCHED_DATE; break;
					case MYSQL_TYPE_TIME:		statistic = STAT_TEXT_TYPE_FETCHED_TIME; break;
					case MYSQL_TYPE_DATETIME:	statistic = STAT_TEXT_TYPE_FETCHED_DATETIME; break;
					case MYSQL_TYPE_YEAR:		statistic = STAT_TEXT_TYPE_FETCHED_YEAR; break;
					case MYSQL_TYPE_NEWDATE:	statistic = STAT_TEXT_TYPE_FETCHED_DATE; break;
					case MYSQL_TYPE_VARCHAR:	statistic = STAT_TEXT_TYPE_FETCHED_STRING; break;
					case MYSQL_TYPE_BIT:		statistic = STAT_TEXT_TYPE_FETCHED_BIT; break;
					case MYSQL_TYPE_NEWDECIMAL:	statistic = STAT_TEXT_TYPE_FETCHED_DECIMAL; break;
					case MYSQL_TYPE_ENUM:		statistic = STAT_TEXT_TYPE_FETCHED_ENUM; break;
					case MYSQL_TYPE_SET:		statistic = STAT_TEXT_TYPE_FETCHED_SET; break;
					case MYSQL_TYPE_JSON:		statistic = STAT_TEXT_TYPE_FETCHED_JSON; break;
					case MYSQL_TYPE_TINY_BLOB:	statistic = STAT_TEXT_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_MEDIUM_BLOB:statistic = STAT_TEXT_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_LONG_BLOB:	statistic = STAT_TEXT_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_BLOB:		statistic = STAT_TEXT_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_VAR_STRING:	statistic = STAT_TEXT_TYPE_FETCHED_STRING; break;
					case MYSQL_TYPE_STRING:		statistic = STAT_TEXT_TYPE_FETCHED_STRING; break;
					case MYSQL_TYPE_GEOMETRY:	statistic = STAT_TEXT_TYPE_FETCHED_GEOMETRY; break;
					default: statistic = STAT_TEXT_TYPE_FETCHED_OTHER; break;
				}
				MYSQLND_INC_CONN_STATISTIC_W_VALUE2(stats, statistic, 1, STAT_BYTES_RECEIVED_PURE_DATA_TEXT, len);
			}
			if (fields_metadata[i].type == MYSQL_TYPE_BIT) {
				/*
				  BIT fields are specially handled. As they come as bit mask, they have
				  to be converted to human-readable representation.
				*/
				ps_fetch_from_1_to_8_bytes(current_field, &(fields_metadata[i]), 0, (const zend_uchar **) &p, len);
				/*
				  We have advanced in ps_fetch_from_1_to_8_bytes. We should go back because
				  later in this function there will be an advancement.
				*/
				p -= len;
				if (Z_TYPE_P(current_field) == IS_LONG && !as_int_or_float) {
					/* we are using the text protocol, so convert to string */
					char tmp[22];
					const size_t tmp_len = sprintf((char *)&tmp, ZEND_ULONG_FMT, Z_LVAL_P(current_field));
					ZVAL_STRINGL(current_field, tmp, tmp_len);
				} else if (Z_TYPE_P(current_field) == IS_STRING) {
					/* nothing to do here, as we want a string and ps_fetch_from_1_to_8_bytes() has given us one */
				}
			}
#ifdef MYSQLND_STRING_TO_INT_CONVERSION
			else if (as_int_or_float && perm_bind.php_type == IS_LONG) {
				zend_uchar save = *(p + len);
				/* We have to make it ASCIIZ temporarily */
				*(p + len) = '\0';
				if (perm_bind.pack_len < SIZEOF_ZEND_LONG) {
					/* direct conversion */
					int64_t v =
#ifndef PHP_WIN32
						atoll((char *) p);
#else
						_atoi64((char *) p);
#endif
					ZVAL_LONG(current_field, (zend_long) v); /* the cast is safe */
				} else {
					uint64_t v =
#ifndef PHP_WIN32
						(uint64_t) atoll((char *) p);
#else
						(uint64_t) _atoi64((char *) p);
#endif
					zend_bool uns = fields_metadata[i].flags & UNSIGNED_FLAG? TRUE:FALSE;
					/* We have to make it ASCIIZ temporarily */
#if SIZEOF_ZEND_LONG==8
					if (uns == TRUE && v > 9223372036854775807L)
#elif SIZEOF_ZEND_LONG==4
					if ((uns == TRUE && v > L64(2147483647)) ||
						(uns == FALSE && (( L64(2147483647) < (int64_t) v) ||
						(L64(-2147483648) > (int64_t) v))))
#else
#error Need fix for this architecture
#endif /* SIZEOF */
					{
						ZVAL_STRINGL(current_field, (char *)p, len);
					} else {
						ZVAL_LONG(current_field, (zend_long) v); /* the cast is safe */
					}
				}
				*(p + len) = save;
			} else if (as_int_or_float && perm_bind.php_type == IS_DOUBLE) {
				zend_uchar save = *(p + len);
				/* We have to make it ASCIIZ temporarily */
				*(p + len) = '\0';
				ZVAL_DOUBLE(current_field, zend_strtod((char *) p, NULL));
				*(p + len) = save;
			}
#endif /* MYSQLND_STRING_TO_INT_CONVERSION */
			else if (len == 0) {
				ZVAL_EMPTY_STRING(current_field);
			} else if (len == 1) {
				ZVAL_INTERNED_STR(current_field, ZSTR_CHAR((zend_uchar)*(char *)p));
			} else {
				ZVAL_STRINGL(current_field, (char *)p, len);
			}
			p += len;
		}
	}

	DBG_RETURN(PASS);