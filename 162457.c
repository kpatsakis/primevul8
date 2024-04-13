innodb_flush(
/*=========*/
	ENGINE_HANDLE*	handle,		/*!< in: Engine Handle */
	const void*	cookie,		/*!< in: connection cookie */
	time_t		when)		/*!< in: when to flush, not used by
					InnoDB */
{
	struct innodb_engine*	innodb_eng = innodb_handle(handle);
	struct default_engine*	def_eng = default_handle(innodb_eng);
	ENGINE_ERROR_CODE	err = ENGINE_SUCCESS;
	meta_cfg_info_t*	meta_info = innodb_eng->meta_info;
	ib_err_t		ib_err = DB_SUCCESS;
	innodb_conn_data_t*	conn_data;

	if (meta_info->flush_option == META_CACHE_OPT_DISABLE) {
		return(ENGINE_SUCCESS);
	}

	if (meta_info->flush_option == META_CACHE_OPT_DEFAULT
	    || meta_info->flush_option == META_CACHE_OPT_MIX) {
		/* default engine flush */
		err = def_eng->engine.flush(innodb_eng->default_engine,
					    cookie, when);

		if (meta_info->flush_option == META_CACHE_OPT_DEFAULT) {
			return(err);
		}
	}

	/* Lock the whole engine, so no other connection can start
	new opeartion */
        pthread_mutex_lock(&innodb_eng->conn_mutex);

	/* Lock the flush_mutex for blocking other DMLs. */
	pthread_mutex_lock(&innodb_eng->flush_mutex);

	conn_data = innodb_eng->server.cookie->get_engine_specific(cookie);

	if (conn_data) {
		/* Commit any work on this connection */
		innodb_api_cursor_reset(innodb_eng, conn_data,
					CONN_OP_FLUSH, true);
	}

        conn_data = innodb_conn_init(innodb_eng, cookie, CONN_MODE_WRITE,
				     IB_LOCK_TABLE_X, true, NULL);

	if (!conn_data) {
		pthread_mutex_unlock(&innodb_eng->flush_mutex);
		pthread_mutex_unlock(&innodb_eng->conn_mutex);
		return(ENGINE_SUCCESS);
	}

	/* Commit any previous work on this connection */
	innodb_api_cursor_reset(innodb_eng, conn_data, CONN_OP_FLUSH, true);

	if (!innodb_flush_sync_conn(innodb_eng, cookie, true)) {
		pthread_mutex_unlock(&innodb_eng->flush_mutex);
		pthread_mutex_unlock(&innodb_eng->conn_mutex);
		innodb_flush_sync_conn(innodb_eng, cookie, false);
		return(ENGINE_TMPFAIL);
	}

	ib_err = innodb_api_flush(innodb_eng, conn_data,
				  meta_info->col_info[CONTAINER_DB].col_name,
			          meta_info->col_info[CONTAINER_TABLE].col_name);

	/* Commit work and release the MDL table. */
	innodb_api_cursor_reset(innodb_eng, conn_data, CONN_OP_FLUSH, true);
	innodb_conn_clean_data(conn_data, false, false);

        pthread_mutex_unlock(&innodb_eng->flush_mutex);
        pthread_mutex_unlock(&innodb_eng->conn_mutex);

	innodb_flush_sync_conn(innodb_eng, cookie, false);

	return((ib_err == DB_SUCCESS) ? ENGINE_SUCCESS : ENGINE_TMPFAIL);
}