innodb_arithmetic(
/*==============*/
	ENGINE_HANDLE*	handle,		/*!< in: Engine Handle */
	const void*	cookie,		/*!< in: connection cookie */
	const void*	key,		/*!< in: key for the value to add */
	const int	nkey,		/*!< in: key length */
	const bool	increment,	/*!< in: whether to increment
					or decrement */
	const bool	create,		/*!< in: whether to create the key
					value pair if can't find */
	const uint64_t	delta,		/*!< in: value to add/substract */
	const uint64_t	initial,	/*!< in: initial */
	const rel_time_t exptime,	/*!< in: expiration time */
	uint64_t*	cas,		/*!< out: new cas value */
	uint64_t*	result,		/*!< out: result value */
	uint16_t	vbucket)	/*!< in: bucket, used by default
					engine only */
{
	struct innodb_engine*	innodb_eng = innodb_handle(handle);
	struct default_engine*	def_eng = default_handle(innodb_eng);
	innodb_conn_data_t*	conn_data;
	meta_cfg_info_t*	meta_info = innodb_eng->meta_info;
	ENGINE_ERROR_CODE	err_ret;

	if (meta_info->set_option == META_CACHE_OPT_DISABLE) {
		return(ENGINE_SUCCESS);
	}

	if (meta_info->set_option == META_CACHE_OPT_DEFAULT
	    || meta_info->set_option == META_CACHE_OPT_MIX) {
		/* For cache-only, forward this to the
		default engine */
		err_ret = def_eng->engine.arithmetic(
			innodb_eng->default_engine, cookie, key, nkey,
			increment, create, delta, initial, exptime, cas,
			result, vbucket);

		if (meta_info->set_option == META_CACHE_OPT_DEFAULT) {
			return(err_ret);
		}
	}

	conn_data = innodb_conn_init(innodb_eng, cookie, CONN_MODE_WRITE,
				     IB_LOCK_X, false, NULL);

	if (!conn_data) {
		return(ENGINE_NOT_STORED);
	}

	err_ret = innodb_api_arithmetic(innodb_eng, conn_data, key, nkey,
					delta, increment, cas, exptime,
					create, initial, result);

	innodb_api_cursor_reset(innodb_eng, conn_data, CONN_OP_WRITE,
				true);

	return(err_ret);
}