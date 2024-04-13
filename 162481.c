create_instance(
/*============*/
	uint64_t		interface,	/*!< in: protocol version,
						currently always 1 */
	GET_SERVER_API		get_server_api,	/*!< in: Callback the engines
						may call to get the public
						server interface */
	ENGINE_HANDLE**		handle )	/*!< out: Engine handle */
{
	ENGINE_ERROR_CODE	err_ret;
	struct innodb_engine*	innodb_eng;

	SERVER_HANDLE_V1 *api = get_server_api();

	if (interface != 1 || api == NULL) {
		return(ENGINE_ENOTSUP);
	}

	innodb_eng = malloc(sizeof(struct innodb_engine));

	if (innodb_eng == NULL) {
		return(ENGINE_ENOMEM);
	}

	memset(innodb_eng, 0, sizeof(*innodb_eng));
	innodb_eng->engine.interface.interface = 1;
	innodb_eng->engine.get_info = innodb_get_info;
	innodb_eng->engine.initialize = innodb_initialize;
	innodb_eng->engine.destroy = innodb_destroy;
	innodb_eng->engine.allocate = innodb_allocate;
	innodb_eng->engine.remove = innodb_remove;
	innodb_eng->engine.release = innodb_release;
	innodb_eng->engine.clean_engine= innodb_clean_engine;
	innodb_eng->engine.get = innodb_get;
	innodb_eng->engine.get_stats = innodb_get_stats;
	innodb_eng->engine.reset_stats = innodb_reset_stats;
	innodb_eng->engine.store = innodb_store;
	innodb_eng->engine.arithmetic = innodb_arithmetic;
	innodb_eng->engine.flush = innodb_flush;
	innodb_eng->engine.unknown_command = innodb_unknown_command;
	innodb_eng->engine.item_set_cas = item_set_cas;
	innodb_eng->engine.get_item_info = innodb_get_item_info;
	innodb_eng->engine.get_stats_struct = NULL;
	innodb_eng->engine.errinfo = NULL;
	innodb_eng->engine.bind = innodb_bind;

	innodb_eng->server = *api;
	innodb_eng->get_server_api = get_server_api;

	/* configuration, with default values*/
	innodb_eng->info.info.description = "InnoDB Memcache " VERSION;
	innodb_eng->info.info.num_features = 3;
	innodb_eng->info.info.features[0].feature = ENGINE_FEATURE_CAS;
	innodb_eng->info.info.features[1].feature =
		ENGINE_FEATURE_PERSISTENT_STORAGE;
	innodb_eng->info.info.features[0].feature = ENGINE_FEATURE_LRU;

	/* Now call create_instace() for the default engine */
	err_ret = create_my_default_instance(interface, get_server_api,
				       &(innodb_eng->default_engine));

	if (err_ret != ENGINE_SUCCESS) {
		free(innodb_eng);
		return(err_ret);
	}

	innodb_eng->clean_stale_conn = false;
	innodb_eng->initialized = true;

	*handle = (ENGINE_HANDLE*) &innodb_eng->engine;

	return(ENGINE_SUCCESS);
}