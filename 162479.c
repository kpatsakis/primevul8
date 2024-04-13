innodb_initialize(
/*==============*/
	ENGINE_HANDLE*	handle,		/*!< in/out: InnoDB memcached
					engine */
	const char*	config_str)	/*!< in: configure string */
{
	ENGINE_ERROR_CODE	return_status = ENGINE_SUCCESS;
	struct innodb_engine*	innodb_eng = innodb_handle(handle);
	struct default_engine*	def_eng = default_handle(innodb_eng);
	eng_config_info_t*	my_eng_config;
	pthread_attr_t          attr;

	my_eng_config = (eng_config_info_t*) config_str;

	/* If no call back function registered (InnoDB engine failed to load),
	load InnoDB Memcached engine should fail too */
	if (!my_eng_config->cb_ptr) {
		return(ENGINE_TMPFAIL);
	}

	/* Register the call back function */
	register_innodb_cb((void*) my_eng_config->cb_ptr);

	innodb_eng->read_batch_size = (my_eng_config->eng_read_batch_size
					? my_eng_config->eng_read_batch_size
					: CONN_NUM_READ_COMMIT);

	innodb_eng->write_batch_size = (my_eng_config->eng_write_batch_size
					? my_eng_config->eng_write_batch_size
					: CONN_NUM_WRITE_COMMIT);

	innodb_eng->enable_binlog = my_eng_config->eng_enable_binlog;

	innodb_eng->cfg_status = innodb_cb_get_cfg();

	/* If binlog is not enabled by InnoDB memcached plugin, let's
	check whether innodb_direct_access_enable_binlog is turned on */
	if (!innodb_eng->enable_binlog) {
		innodb_eng->enable_binlog = innodb_eng->cfg_status
					    & IB_CFG_BINLOG_ENABLED;
	}

	innodb_eng->enable_mdl = innodb_eng->cfg_status & IB_CFG_MDL_ENABLED;
	innodb_eng->trx_level = ib_cb_cfg_trx_level();
	innodb_eng->bk_commit_interval = ib_cb_cfg_bk_commit_interval();

	UT_LIST_INIT(innodb_eng->conn_data);
	pthread_mutex_init(&innodb_eng->conn_mutex, NULL);
	pthread_mutex_init(&innodb_eng->cas_mutex, NULL);
	pthread_mutex_init(&innodb_eng->flush_mutex, NULL);

	/* Fetch InnoDB specific settings */
	innodb_eng->meta_info = innodb_config(
		NULL, 0, &innodb_eng->meta_hash);

	if (!innodb_eng->meta_info) {
		return(ENGINE_TMPFAIL);
	}

	if (innodb_eng->default_engine) {
		return_status = def_eng->engine.initialize(
			innodb_eng->default_engine,
			my_eng_config->option_string);
	}

	memcached_shutdown = false;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        pthread_create(&innodb_eng->bk_thd_for_commit, &attr, innodb_bk_thread,
		       handle);

	return(return_status);
}