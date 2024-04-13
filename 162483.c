innodb_allocate(
/*============*/
	ENGINE_HANDLE*	handle,		/*!< in: Engine handle */
	const void*	cookie,		/*!< in: connection cookie */
	item **		item,		/*!< out: item to allocate */
	const void*	key,		/*!< in: key */
	const size_t	nkey,		/*!< in: key length */
	const size_t	nbytes,		/*!< in: estimated value length */
	const int	flags,		/*!< in: flag */
	const rel_time_t exptime)	/*!< in: expiration time */
{
	size_t			len;
	struct innodb_engine*	innodb_eng = innodb_handle(handle);
	struct default_engine*	def_eng = default_handle(innodb_eng);
	innodb_conn_data_t*	conn_data;
	hash_item*		it = NULL;
	meta_cfg_info_t*	meta_info = innodb_eng->meta_info;


	conn_data = innodb_eng->server.cookie->get_engine_specific(cookie);

	if (!conn_data) {
		conn_data = innodb_conn_init(innodb_eng, cookie,
					     CONN_MODE_WRITE,
					     IB_LOCK_X, false, NULL);
		if (!conn_data) {
			return(ENGINE_TMPFAIL);
		}
	}

	/* If system configured to use Memcached default engine (instead
	of InnoDB engine), continue to use Memcached's default memory
	allocation */
	if (meta_info->set_option == META_CACHE_OPT_DEFAULT
            || meta_info->set_option == META_CACHE_OPT_MIX) {
		conn_data->use_default_mem = true;
		conn_data->in_use = false;
		return(def_eng->engine.allocate(
			innodb_eng->default_engine,
			cookie, item, key, nkey, nbytes,
			flags, exptime));
	}

	conn_data->use_default_mem = false;
	len = sizeof(*it) + nkey + nbytes + sizeof(uint64_t);
	if (len > conn_data->cmd_buf_len) {
		free(conn_data->cmd_buf);
		conn_data->cmd_buf = malloc(len);
		conn_data->cmd_buf_len = len;
	}

	it = (hash_item*) conn_data->cmd_buf;

	it->next = it->prev = it->h_next = 0;
	it->refcount = 1;
	it->iflag = def_eng->config.use_cas ? ITEM_WITH_CAS : 0;
	it->nkey = nkey;
	it->nbytes = nbytes;
	it->flags = flags;
	it->slabs_clsid = 1;
	/* item_get_key() is a memcached code, here we cast away const return */
	memcpy((void*) item_get_key(it), key, nkey);
	it->exptime = exptime;

	*item = it;
	conn_data->in_use = false;

	return(ENGINE_SUCCESS);
}