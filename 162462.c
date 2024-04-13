innodb_destroy(
/*===========*/
	ENGINE_HANDLE*	handle,		/*!< in: Destroy the engine instance */
	bool		force)		/*!< in: Force to destroy */
{
	struct innodb_engine* innodb_eng = innodb_handle(handle);
	struct default_engine *def_eng = default_handle(innodb_eng);

	memcached_shutdown = true;

	/* Wait for the background thread to exit */
	while (!bk_thd_exited) {
		sleep(1);
	}

	innodb_conn_clean(innodb_eng, true, false);

	if (innodb_eng->meta_hash) {
		HASH_CLEANUP(innodb_eng->meta_hash, meta_cfg_info_t*);
	}

	pthread_mutex_destroy(&innodb_eng->conn_mutex);
	pthread_mutex_destroy(&innodb_eng->cas_mutex);
	pthread_mutex_destroy(&innodb_eng->flush_mutex);

	if (innodb_eng->default_engine) {
		def_eng->engine.destroy(innodb_eng->default_engine, force);
	}

	free(innodb_eng);
}