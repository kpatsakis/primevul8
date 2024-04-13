void pb_controller::run(int argc, char * argv[]) {
	int c;
	bool automatic_dl = false;

	::signal(SIGINT, ctrl_c_action);

	do {
		if ((c = ::getopt(argc, argv, "C:q:d:l:ha")) < 0)
			continue;

		switch (c) {
		case ':':
		case '?':
			usage(argv[0]);
			break;
		case 'C':
			config_file = optarg;
			break;
		case 'q':
			queue_file = optarg;
			break;
		case 'a':
			automatic_dl = true;
			break;
		case 'd': // this is an undocumented debug commandline option!
			logger::getInstance().set_logfile(optarg);
			break;
		case 'l': { // this is an undocumented debug commandline option!
			loglevel level = static_cast<loglevel>(atoi(optarg));
			if (level > LOG_NONE && level <= LOG_DEBUG)
				logger::getInstance().set_loglevel(level);
		}
		break;
		case 'h':
			usage(argv[0]);
			break;
		default:
			std::cout << utils::strprintf(_("%s: unknown option - %c"), argv[0], static_cast<char>(c)) << std::endl;
			usage(argv[0]);
			break;
		}
	} while (c != -1);

	std::cout << utils::strprintf(_("Starting %s %s..."), "podbeuter", PROGRAM_VERSION) << std::endl;

	pid_t pid;
	if (!utils::try_fs_lock(lock_file, pid)) {
		std::cout << utils::strprintf(_("Error: an instance of %s is already running (PID: %u)"), "podbeuter", pid) << std::endl;
		return;
	}

	std::cout << _("Loading configuration...");
	std::cout.flush();

	configparser cfgparser;
	cfg = new configcontainer();
	cfg->register_commands(cfgparser);
	colormanager * colorman = new colormanager();
	colorman->register_commands(cfgparser);

	keymap keys(KM_PODBEUTER);
	cfgparser.register_handler("bind-key", &keys);
	cfgparser.register_handler("unbind-key", &keys);

	null_config_action_handler null_cah;
	cfgparser.register_handler("macro", &null_cah);
	cfgparser.register_handler("ignore-article", &null_cah);
	cfgparser.register_handler("always-download", &null_cah);
	cfgparser.register_handler("define-filter", &null_cah);
	cfgparser.register_handler("highlight", &null_cah);
	cfgparser.register_handler("highlight-article", &null_cah);
	cfgparser.register_handler("reset-unread-on-update", &null_cah);

	try {
		cfgparser.parse("/etc/newsbeuter/config");
		cfgparser.parse(config_file);
	} catch (const configexception& ex) {
		std::cout << ex.what() << std::endl;
		delete colorman;
		return;
	}

	if (colorman->colors_loaded())
		colorman->set_pb_colors(v);
	delete colorman;

	max_dls = cfg->get_configvalue_as_int("max-downloads");

	std::cout << _("done.") << std::endl;

	ql = new queueloader(queue_file, this);
	ql->reload(downloads_);

	v->set_keymap(&keys);

	v->run(automatic_dl);

	stfl::reset();

	std::cout <<  _("Cleaning up queue...");
	std::cout.flush();

	ql->reload(downloads_);
	delete ql;

	std::cout << _("done.") << std::endl;

	utils::remove_fs_lock(lock_file);
}