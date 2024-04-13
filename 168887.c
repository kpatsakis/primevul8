void pb_controller::usage(const char * argv0) {
	std::cout << utils::strprintf(_("%s %s\nusage %s [-C <file>] [-q <file>] [-h]\n"
	                                "-C <configfile> read configuration from <configfile>\n"
	                                "-q <queuefile>  use <queuefile> as queue file\n"
	                                "-a              start download on startup\n"
	                                "-h              this help\n"), "podbeuter", PROGRAM_VERSION, argv0);
	::exit(EXIT_FAILURE);
}