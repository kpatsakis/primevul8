set_std_fd(bool force)
{
	int fd;

	if (force || __test_bit(DONT_FORK_BIT, &debug)) {
		fd = open("/dev/null", O_RDWR);
		if (fd != -1) {
			dup2(fd, STDIN_FILENO);
			dup2(fd, STDOUT_FILENO);
			dup2(fd, STDERR_FILENO);
			if (fd > STDERR_FILENO)
				close(fd);
		}
	}

	signal_fd_close(STDERR_FILENO+1);
}