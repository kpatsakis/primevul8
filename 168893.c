bool pb_controller::setup_dirs_xdg(const char *env_home) {
	const char *env_xdg_config;
	const char *env_xdg_data;
	std::string xdg_config_dir;
	std::string xdg_data_dir;

	env_xdg_config = ::getenv("XDG_CONFIG_HOME");
	if (env_xdg_config) {
		xdg_config_dir = env_xdg_config;
	} else {
		xdg_config_dir = env_home;
		xdg_config_dir.append(NEWSBEUTER_PATH_SEP);
		xdg_config_dir.append(".config");
	}

	env_xdg_data = ::getenv("XDG_DATA_HOME");
	if (env_xdg_data) {
		xdg_data_dir = env_xdg_data;
	} else {
		xdg_data_dir = env_home;
		xdg_data_dir.append(NEWSBEUTER_PATH_SEP);
		xdg_data_dir.append(".local");
		xdg_data_dir.append(NEWSBEUTER_PATH_SEP);
		xdg_data_dir.append("share");
	}

	xdg_config_dir.append(NEWSBEUTER_PATH_SEP);
	xdg_config_dir.append(NEWSBEUTER_SUBDIR_XDG);

	xdg_data_dir.append(NEWSBEUTER_PATH_SEP);
	xdg_data_dir.append(NEWSBEUTER_SUBDIR_XDG);

	if (access(xdg_config_dir.c_str(), R_OK | X_OK) != 0) {
		std::cout << utils::strprintf(_("XDG: configuration directory '%s' not accessible, using '%s' instead."), xdg_config_dir.c_str(), config_dir.c_str()) << std::endl;
		return false;
	}
	if (access(xdg_data_dir.c_str(), R_OK | X_OK | W_OK) != 0) {
		std::cout << utils::strprintf(_("XDG: data directory '%s' not accessible, using '%s' instead."), xdg_data_dir.c_str(), config_dir.c_str()) << std::endl;
		return false;
	}

	config_dir = xdg_config_dir;

	/* in config */
	url_file = config_dir + std::string(NEWSBEUTER_PATH_SEP) + url_file;
	config_file = config_dir + std::string(NEWSBEUTER_PATH_SEP) + config_file;

	/* in data */
	cache_file = xdg_data_dir + std::string(NEWSBEUTER_PATH_SEP) + cache_file;
	lock_file = cache_file + LOCK_SUFFIX;
	queue_file = xdg_data_dir + std::string(NEWSBEUTER_PATH_SEP) + queue_file;
	searchfile = utils::strprintf("%s%shistory.search", xdg_data_dir.c_str(), NEWSBEUTER_PATH_SEP);
	cmdlinefile = utils::strprintf("%s%shistory.cmdline", xdg_data_dir.c_str(), NEWSBEUTER_PATH_SEP);

	return true;
}