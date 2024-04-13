innodb_get(
/*=======*/
	ENGINE_HANDLE*		handle,		/*!< in: Engine Handle */
	const void*		cookie,		/*!< in: connection cookie */
	item**			item,		/*!< out: item to fill */
	const void*		key,		/*!< in: search key */
	const int		nkey,		/*!< in: key length */
	uint16_t		vbucket __attribute__((unused)))
						/*!< in: bucket, used by default
						engine only */
{
	struct innodb_engine*	innodb_eng = innodb_handle(handle);
	ib_crsr_t		crsr;
	ib_err_t		err = DB_SUCCESS;
	mci_item_t*		result = NULL;
	ENGINE_ERROR_CODE	err_ret = ENGINE_SUCCESS;
	innodb_conn_data_t*	conn_data = NULL;
	meta_cfg_info_t*	meta_info = innodb_eng->meta_info;
	int			option_length;
	const char*		option_delimiter;
	size_t			key_len = nkey;
	int			lock_mode;
	bool			report_table_switch = false;

	if (meta_info->get_option == META_CACHE_OPT_DISABLE) {
		return(ENGINE_KEY_ENOENT);
	}

	if (meta_info->get_option == META_CACHE_OPT_DEFAULT
	    || meta_info->get_option == META_CACHE_OPT_MIX) {
		*item = item_get(default_handle(innodb_eng), key, nkey);

		if (*item != NULL) {
			return(ENGINE_SUCCESS);
		}

		if (meta_info->get_option == META_CACHE_OPT_DEFAULT) {
			return(ENGINE_KEY_ENOENT);
		}
	}

	/* Check if we need to switch table mapping */
	err_ret = check_key_name_for_map_switch(handle, cookie, key, &key_len);

	/* If specified new table map does not exist, or table does not
	qualify for InnoDB memcached, return error */
	if (err_ret != ENGINE_SUCCESS) {
		goto err_exit;
	}

	/* If only the new mapping name is provided, and no key value,
	return here */
	if (key_len <= 0) {
		/* If this is a command in the form of "get @@new_table_map",
		for the purpose of switching to the specified table with
		the table map name, if the switch is successful, we will
		return the table name as result */
		if (nkey > 0) {
			report_table_switch = true;

			goto search_done;
		}

		err_ret = ENGINE_KEY_ENOENT;
		goto err_exit;
	}

	lock_mode = (innodb_eng->trx_level == IB_TRX_SERIALIZABLE
		     && innodb_eng->read_batch_size == 1)
			? IB_LOCK_S
			: IB_LOCK_NONE;

	conn_data = innodb_conn_init(innodb_eng, cookie, CONN_MODE_READ,
				     lock_mode, false, NULL);

	if (!conn_data) {
		return(ENGINE_TMPFAIL);
	}

	result = (mci_item_t*)(conn_data->result);

	err = innodb_api_search(conn_data, &crsr, key + nkey - key_len,
				key_len, result, NULL, true);

	if (err != DB_SUCCESS) {
		err_ret = ENGINE_KEY_ENOENT;
		goto func_exit;
	}

search_done:
	if (report_table_switch) {
		char	table_name[MAX_TABLE_NAME_LEN
				   + MAX_DATABASE_NAME_LEN];
		char*	name;
		char*	dbname;

		conn_data = innodb_eng->server.cookie->get_engine_specific(cookie);
		assert(nkey > 0);

		name = conn_data->conn_meta->col_info[CONTAINER_TABLE].col_name;
		dbname = conn_data->conn_meta->col_info[CONTAINER_DB].col_name;
#ifdef __WIN__
		sprintf(table_name, "%s\%s", dbname, name);
#else
		snprintf(table_name, sizeof(table_name),
			 "%s/%s", dbname, name);
#endif

		assert(!conn_data->result_in_use);
		conn_data->result_in_use = true;
		result = (mci_item_t*)(conn_data->result);

		memset(result, 0, sizeof(*result));

		memcpy(conn_data->row_buf, table_name, strlen(table_name));

		result->col_value[MCI_COL_VALUE].value_str = conn_data->row_buf;
		result->col_value[MCI_COL_VALUE].value_len = strlen(table_name);
	}

	result->col_value[MCI_COL_KEY].value_str = (char*)key;
	result->col_value[MCI_COL_KEY].value_len = nkey;

	/* Only if expiration field is enabled, and the value is not zero,
	we will check whether the item is expired */
	if (result->col_value[MCI_COL_EXP].is_valid
	    && result->col_value[MCI_COL_EXP].value_int) {
		uint64_t time;
		time = mci_get_time();
		if (time > result->col_value[MCI_COL_EXP].value_int) {
			innodb_free_item(result);
			err_ret = ENGINE_KEY_ENOENT;
			goto func_exit;
		}
	}

	if (result->extra_col_value) {
		int		i;
		char*		c_value;
		char*		value_end;
		unsigned int	total_len = 0;
		char		int_buf[MAX_INT_CHAR_LEN];

		GET_OPTION(meta_info, OPTION_ID_COL_SEP, option_delimiter,
			   option_length);

		assert(option_length > 0 && option_delimiter);

		for (i = 0; i < result->n_extra_col; i++) {
			mci_column_t*   mci_item = &result->extra_col_value[i];

			if (mci_item->value_len == 0) {
				total_len += option_length;
				continue;
			}

			if (!mci_item->is_str) {
				memset(int_buf, 0, sizeof int_buf);
				assert(!mci_item->value_str);

				total_len += convert_to_char(
					int_buf, sizeof int_buf,
					&mci_item->value_int,
					mci_item->value_len,
					mci_item->is_unsigned);
			} else {
				total_len += result->extra_col_value[i].value_len;
			}

			total_len += option_length;
		}

		/* No need to add the last separator */
		total_len -= option_length;

		if (total_len > conn_data->mul_col_buf_len) {
			if (conn_data->mul_col_buf) {
				free(conn_data->mul_col_buf);
			}

			conn_data->mul_col_buf = malloc(total_len + 1);
			conn_data->mul_col_buf_len = total_len;
		}

		c_value = conn_data->mul_col_buf;
		value_end = conn_data->mul_col_buf + total_len;

		for (i = 0; i < result->n_extra_col; i++) {
			mci_column_t*   col_value;

			col_value = &result->extra_col_value[i];

			if (col_value->value_len != 0) {
				if (!col_value->is_str) {
					int	int_len;
					memset(int_buf, 0, sizeof int_buf);

					int_len = convert_to_char(
						int_buf,
						sizeof int_buf,
						&col_value->value_int,
						col_value->value_len,
						col_value->is_unsigned);

                                        assert(int_len <= conn_data->mul_col_buf_len);

					memcpy(c_value, int_buf, int_len);
					c_value += int_len;
				} else {
					memcpy(c_value,
					       col_value->value_str,
					       col_value->value_len);
					c_value += col_value->value_len;
				}
			}

			if (i < result->n_extra_col - 1 ) {
				memcpy(c_value, option_delimiter, option_length);
				c_value += option_length;
			}

			assert(c_value <= value_end);

			if (col_value->allocated) {
				free(col_value->value_str);
			}
		}

		result->col_value[MCI_COL_VALUE].value_str = conn_data->mul_col_buf;
		result->col_value[MCI_COL_VALUE].value_len = total_len;
		((char*)result->col_value[MCI_COL_VALUE].value_str)[total_len] = 0;

		free(result->extra_col_value);
	} else if (!result->col_value[MCI_COL_VALUE].is_str
		&& result->col_value[MCI_COL_VALUE].value_len != 0) {
		unsigned int	int_len;
		char		int_buf[MAX_INT_CHAR_LEN];

		int_len = convert_to_char(
			int_buf, sizeof int_buf,
			&result->col_value[MCI_COL_VALUE].value_int,
			result->col_value[MCI_COL_VALUE].value_len,
			result->col_value[MCI_COL_VALUE].is_unsigned);

		if (int_len > conn_data->mul_col_buf_len) {
			if (conn_data->mul_col_buf) {
				free(conn_data->mul_col_buf);
			}

			conn_data->mul_col_buf = malloc(int_len + 1);
			conn_data->mul_col_buf_len = int_len;
		}

		memcpy(conn_data->mul_col_buf, int_buf, int_len);
		result->col_value[MCI_COL_VALUE].value_str =
			 conn_data->mul_col_buf;

		result->col_value[MCI_COL_VALUE].value_len = int_len;
	}

        *item = result;

func_exit:

	if (!report_table_switch) {
		innodb_api_cursor_reset(innodb_eng, conn_data,
					CONN_OP_READ, true);
	}

err_exit:

	/* If error return, memcached will not call InnoDB Memcached's
	callback function "innodb_release" to reset the result_in_use
	value. So we reset it here */
	if (err_ret != ENGINE_SUCCESS && conn_data) {
		conn_data->result_in_use = false;
	}
	return(err_ret);
}