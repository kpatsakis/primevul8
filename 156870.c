run_perf(const char *process, const char *network_namespace, const char *instance_name)
{
	int ret;
	pid_t pid;
	char *orig_name = NULL;
	char *new_name;
	const char *perf_name = "perf.data";
	int in = -1;
	int ep = -1;

	do {
		orig_name = MALLOC(PATH_MAX);
		if (!getcwd(orig_name, PATH_MAX)) {
			log_message(LOG_INFO, "Unable to get cwd");
			break;
		}

#ifdef IN_CLOEXEC
		in = inotify_init1(IN_CLOEXEC | IN_NONBLOCK);
#else
		if ((in = inotify_init()) != -1) {
			fcntl(in, F_SETFD, FD_CLOEXEC | fcntl(n, F_GETFD));
			fcntl(in, F_SETFL, O_NONBLOCK | fcntl(n, F_GETFL));
		}
#endif
		if (in == -1) {
			log_message(LOG_INFO, "inotify_init failed %d - %m", errno);
			break;
		}

		if (inotify_add_watch(in, orig_name, IN_CREATE) == -1) {
			log_message(LOG_INFO, "inotify_add_watch of %s failed %d - %m", orig_name, errno);
			break;
		}

		pid = fork();

		if (pid == -1) {
			log_message(LOG_INFO, "fork() for perf failed");
			break;
		}

		/* Child */
		if (!pid) {
			char buf[9];

			snprintf(buf, sizeof buf, "%d", getppid());
			execlp("perf", "perf", "record", "-p", buf, "-q", "-g", "--call-graph", "fp", NULL);
			exit(0);
		}

		/* Parent */
		char buf[sizeof(struct inotify_event) + NAME_MAX + 1];
		struct inotify_event *ie = (struct inotify_event*)buf;
		struct epoll_event ee = { .events = EPOLLIN, .data.fd = in };

		if ((ep = epoll_create(1)) == -1) {
			log_message(LOG_INFO, "perf epoll_create failed errno %d - %m", errno);
			break;
		}

		if (epoll_ctl(ep, EPOLL_CTL_ADD, in, &ee) == -1) {
			log_message(LOG_INFO, "perf epoll_ctl failed errno %d - %m", errno);
			break;
		}

		do {
			ret = epoll_wait(ep, &ee, 1, 1000);
			if (ret == 0) {
				log_message(LOG_INFO, "Timed out waiting for creation of %s", perf_name);
				break;
			}
			else if (ret == -1) {
				if (errno == EINTR)
					continue;
				log_message(LOG_INFO, "perf epoll returned errno %d - %m", errno);
				break;
			}

			ret = read(in, buf, sizeof(buf));
			if (ret == -1) {
				if (errno == EINTR)
					continue;
				log_message(LOG_INFO, "perf inotify read returned errno %d %m", errno);
				break;
			}
			if (ret < (int)sizeof(*ie)) {
				log_message(LOG_INFO, "read returned %d", ret);
				break;
			}
			if (!(ie->mask & IN_CREATE)) {
				log_message(LOG_INFO, "mask is 0x%x", ie->mask);
				continue;
			}
			if (!ie->len) {
				log_message(LOG_INFO, "perf inotify read returned no len");
				continue;
			}

			if (strcmp(ie->name, perf_name))
				continue;

			/* Rename the /perf.data file */
			strcat(orig_name, perf_name);
			new_name = make_file_name(orig_name, process,
#if HAVE_DECL_CLONE_NEWNET
							network_namespace,
#else
							NULL,
#endif
							instance_name);

			if (rename(orig_name, new_name))
				log_message(LOG_INFO, "Rename %s to %s failed - %m (%d)", orig_name, new_name, errno);

			FREE(new_name);
		} while (false);
	} while (false);

	if (ep != -1)
		close(ep);
	if (in != -1)
		close(in);
	if (orig_name)
		FREE(orig_name);
}