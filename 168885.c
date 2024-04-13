void pb_controller::start_downloads() {
	int dl2start = get_maxdownloads() - downloads_in_progress();
	for (auto it=downloads_.begin(); dl2start > 0 && it!=downloads_.end(); ++it) {
		if (it->status() == DL_QUEUED) {
			std::thread t {poddlthread(&(*it), cfg)};
			--dl2start;
		}
	}
}