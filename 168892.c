void queueloader::reload(std::vector<download>& downloads, bool remove_unplayed) {
	std::vector<download> dltemp;
	std::fstream f;

	for (auto dl : downloads) {
		if (dl.status() == DL_DOWNLOADING) { // we are not allowed to reload if a download is in progress!
			LOG(LOG_INFO, "queueloader::reload: aborting reload due to DL_DOWNLOADING status");
			return;
		}
		switch (dl.status()) {
		case DL_QUEUED:
		case DL_CANCELLED:
		case DL_FAILED:
		case DL_ALREADY_DOWNLOADED:
		case DL_READY:
			LOG(LOG_DEBUG, "queueloader::reload: storing %s to new vector", dl.url());
			dltemp.push_back(dl);
			break;
		case DL_PLAYED:
		case DL_FINISHED:
			if (!remove_unplayed) {
				LOG(LOG_DEBUG, "queueloader::reload: storing %s to new vector", dl.url());
				dltemp.push_back(dl);
			}
			break;
		default:
			break;
		}
	}

	f.open(queuefile.c_str(), std::fstream::in);
	if (f.is_open()) {
		std::string line;
		do {
			getline(f, line);
			if (!f.eof() && line.length() > 0) {
				LOG(LOG_DEBUG, "queueloader::reload: loaded `%s' from queue file", line.c_str());
				std::vector<std::string> fields = utils::tokenize_quoted(line);
				bool url_found = false;

				for (auto dl : dltemp) {
					if (fields[0] == dl.url()) {
						LOG(LOG_INFO, "queueloader::reload: found `%s' in old vector", fields[0].c_str());
						url_found = true;
						break;
					}
				}

				for (auto dl : downloads) {
					if (fields[0] == dl.url()) {
						LOG(LOG_INFO, "queueloader::reload: found `%s' in new vector", line.c_str());
						url_found = true;
						break;
					}
				}

				if (!url_found) {
					LOG(LOG_INFO, "queueloader::reload: found `%s' nowhere -> storing to new vector", line.c_str());
					download d(ctrl);
					std::string fn;
					if (fields.size() == 1)
						fn = get_filename(fields[0]);
					else
						fn = fields[1];
					d.set_filename(fn);
					if (access(fn.c_str(), F_OK)==0) {
						LOG(LOG_INFO, "queueloader::reload: found `%s' on file system -> mark as already downloaded", fn.c_str());
						if (fields.size() >= 3) {
							if (fields[2] == "downloaded")
								d.set_status(DL_READY);
							if (fields[2] == "played")
								d.set_status(DL_PLAYED);
						} else
							d.set_status(DL_ALREADY_DOWNLOADED); // TODO: scrap DL_ALREADY_DOWNLOADED state
					}
					d.set_url(fields[0]);
					dltemp.push_back(d);
				}
			}
		} while (!f.eof());
		f.close();
	}

	f.open(queuefile.c_str(), std::fstream::out);
	if (f.is_open()) {
		for (auto dl : dltemp) {
			f << dl.url() << " " << stfl::quote(dl.filename());
			if (dl.status() == DL_READY)
				f << " downloaded";
			if (dl.status() == DL_PLAYED)
				f << " played";
			f << std::endl;
		}
		f.close();
	}

	downloads = dltemp;
}