innodb_release(
/*===========*/
	ENGINE_HANDLE*		handle,		/*!< in: Engine handle */
	const void*		cookie __attribute__((unused)),
						/*!< in: connection cookie */
	item*			item __attribute__((unused)))
						/*!< in: item to free */
{
	struct innodb_engine*	innodb_eng = innodb_handle(handle);
	innodb_conn_data_t*	conn_data;

	conn_data = innodb_eng->server.cookie->get_engine_specific(cookie);

	if (!conn_data) {
		return;
	}

	conn_data->result_in_use = false;

	/* If item's memory comes from Memcached default engine, release it
	through Memcached APIs */
	if (conn_data->use_default_mem) {
		struct default_engine*  def_eng = default_handle(innodb_eng);

		item_release(def_eng, (hash_item *) item);
		conn_data->use_default_mem = false;
	}

	return;
}