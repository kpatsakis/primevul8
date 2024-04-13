innodb_store(
/*=========*/
	ENGINE_HANDLE*		handle,		/*!< in: Engine Handle */
	const void*		cookie,		/*!< in: connection cookie */
	item*			item,		/*!< out: result to fill */
	uint64_t*		cas,		/*!< in: cas value */
	ENGINE_STORE_OPERATION	op,		/*!< in: type of operation */
	uint16_t		vbucket	__attribute__((unused)))
						/*!< in: bucket, used by default
						engine only */
{
	struct innodb_engine*	innodb_eng = innodb_handle(handle);
	uint16_t		len = hash_item_get_key_len(item);
	char*			value = hash_item_get_key(item);
	uint64_t		exptime = hash_item_get_exp(item);
	uint64_t		flags = hash_item_get_flag(item);
	ENGINE_ERROR_CODE	result;
	uint64_t		input_cas;
	innodb_conn_data_t*	conn_data;
	meta_cfg_info_t*	meta_info = innodb_eng->meta_info;
	uint32_t		val_len = ((hash_item*)item)->nbytes;
	size_t			key_len = len;
	ENGINE_ERROR_CODE	err_ret = ENGINE_SUCCESS;

	if (meta_info->set_option == META_CACHE_OPT_DISABLE) {
		return(ENGINE_SUCCESS);
	}

	if (meta_info->set_option == META_CACHE_OPT_DEFAULT
	    || meta_info->set_option == META_CACHE_OPT_MIX) {
		result = store_item(default_handle(innodb_eng), item, cas,
				    op, cookie);

		if (meta_info->set_option == META_CACHE_OPT_DEFAULT) {
			return(result);
		}
	}

	err_ret = check_key_name_for_map_switch(handle, cookie,
						value, &key_len);

	if (err_ret != ENGINE_SUCCESS) {
		return(err_ret);
	}

	/* If no key is provided, return here */
	if (key_len <= 0) {
		return(ENGINE_NOT_STORED);
	}

	conn_data = innodb_conn_init(innodb_eng, cookie, CONN_MODE_WRITE,
				     IB_LOCK_X, false, NULL);

	if (!conn_data) {
		return(ENGINE_NOT_STORED);
	}

	input_cas = hash_item_get_cas(item);

	result = innodb_api_store(innodb_eng, conn_data, value + len - key_len,
				  key_len, val_len, exptime, cas, input_cas,
				  flags, op);

	innodb_api_cursor_reset(innodb_eng, conn_data, CONN_OP_WRITE,
				result == ENGINE_SUCCESS);
	return(result);
}