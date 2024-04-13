static void ctrl_c_action(int sig) {
	LOG(LOG_DEBUG,"caugh signal %d",sig);
	stfl::reset();
	utils::remove_fs_lock(lock_file);
	::exit(EXIT_FAILURE);
}