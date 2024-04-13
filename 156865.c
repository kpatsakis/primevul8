open_pipe(int pipe_arr[2])
{
	/* Open pipe */
#ifdef HAVE_PIPE2
	if (pipe2(pipe_arr, O_CLOEXEC | O_NONBLOCK) == -1)
#else
	if (pipe(pipe_arr) == -1)
#endif
		return -1;

#ifndef HAVE_PIPE2
	fcntl(pipe_arr[0], F_SETFL, O_NONBLOCK | fcntl(pipe_arr[0], F_GETFL));
	fcntl(pipe_arr[1], F_SETFL, O_NONBLOCK | fcntl(pipe_arr[1], F_GETFL));

	fcntl(pipe_arr[0], F_SETFD, FD_CLOEXEC | fcntl(pipe_arr[0], F_GETFD));
	fcntl(pipe_arr[1], F_SETFD, FD_CLOEXEC | fcntl(pipe_arr[1], F_GETFD));
#endif

	return 0;
}