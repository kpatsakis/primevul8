pb_controller::pb_controller() : v(0), config_file("config"), queue_file("queue"), cfg(0), view_update_(true),  max_dls(1), ql(0) {
	char * cfgdir;
	if (!(cfgdir = ::getenv("HOME"))) {
		struct passwd * spw = ::getpwuid(::getuid());
		if (spw) {
			cfgdir = spw->pw_dir;
		} else {
			std::cout << _("Fatal error: couldn't determine home directory!") << std::endl;
			std::cout << utils::strprintf(_("Please set the HOME environment variable or add a valid user for UID %u!"), ::getuid()) << std::endl;
			::exit(EXIT_FAILURE);
		}
	}
	config_dir = cfgdir;

	if (setup_dirs_xdg(cfgdir))
		return;

	config_dir.append(NEWSBEUTER_PATH_SEP);
	config_dir.append(NEWSBEUTER_CONFIG_SUBDIR);
	::mkdir(config_dir.c_str(),0700); // create configuration directory if it doesn't exist

	config_file = config_dir + std::string(NEWSBEUTER_PATH_SEP) + config_file;
	queue_file = config_dir + std::string(NEWSBEUTER_PATH_SEP) + queue_file;
	lock_file = config_dir + std::string(NEWSBEUTER_PATH_SEP) + lock_file;
}