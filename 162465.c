innodb_flush_sync_conn(
/*===================*/
	innodb_engine_t*	engine,		/*!< in/out: InnoDB memcached
						engine */
	const void*		cookie,		/*!< in: connection cookie */
	bool			flush_flag)	/*!< in: flush is running or not */
{
	innodb_conn_data_t*	conn_data = NULL;
	innodb_conn_data_t*	curr_conn_data;
	bool			ret = true;

	curr_conn_data = engine->server.cookie->get_engine_specific(cookie);
	assert(curr_conn_data);

	conn_data = UT_LIST_GET_FIRST(engine->conn_data);

	while (conn_data) {
		if (conn_data != curr_conn_data && (!conn_data->is_stale)) {
			if (conn_data->thd) {
				handler_thd_attach(conn_data->thd, NULL);
			}
			LOCK_CURRENT_CONN_IF_NOT_LOCKED(false, conn_data);
			if (flush_flag == false) {
				conn_data->is_flushing = flush_flag;
				UNLOCK_CURRENT_CONN_IF_NOT_LOCKED(false, conn_data);
				conn_data = UT_LIST_GET_NEXT(conn_list, conn_data);
				continue;
			}
			if (!conn_data->in_use) {
				/* Set flushing flag to conn_data for preventing
				it is get by other request.  */
				conn_data->is_flushing = flush_flag;
				UNLOCK_CURRENT_CONN_IF_NOT_LOCKED(false, conn_data);
			} else {
				ret = false;
				UNLOCK_CURRENT_CONN_IF_NOT_LOCKED(false, conn_data);
				break;
			}
		}
		conn_data = UT_LIST_GET_NEXT(conn_list, conn_data);
	}

	if (curr_conn_data->thd) {
		handler_thd_attach(curr_conn_data->thd, NULL);
	}

	return(ret);
}