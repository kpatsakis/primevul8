unix_recv_io(int argc, VALUE *argv, VALUE sock)
{
    VALUE klass, mode;
    rb_io_t *fptr;
    struct iomsg_arg arg;
    struct iovec vec[2];
    char buf[1];

    int fd;
#if FD_PASSING_BY_MSG_CONTROL
    union {
	struct cmsghdr hdr;
	char pad[sizeof(struct cmsghdr)+8+sizeof(int)+8];
    } cmsg;
#endif

    rb_scan_args(argc, argv, "02", &klass, &mode);
    if (argc == 0)
	klass = rb_cIO;
    if (argc <= 1)
	mode = Qnil;

    GetOpenFile(sock, fptr);

    arg.msg.msg_name = NULL;
    arg.msg.msg_namelen = 0;

    vec[0].iov_base = buf;
    vec[0].iov_len = sizeof(buf);
    arg.msg.msg_iov = vec;
    arg.msg.msg_iovlen = 1;

#if FD_PASSING_BY_MSG_CONTROL
    arg.msg.msg_control = (caddr_t)&cmsg;
    arg.msg.msg_controllen = (socklen_t)CMSG_SPACE(sizeof(int));
    arg.msg.msg_flags = 0;
    cmsg.hdr.cmsg_len = (socklen_t)CMSG_LEN(sizeof(int));
    cmsg.hdr.cmsg_level = SOL_SOCKET;
    cmsg.hdr.cmsg_type = SCM_RIGHTS;
    fd = -1;
    memcpy(CMSG_DATA(&cmsg.hdr), &fd, sizeof(int));
#else
    arg.msg.msg_accrights = (caddr_t)&fd;
    arg.msg.msg_accrightslen = sizeof(fd);
    fd = -1;
#endif

    arg.fd = fptr->fd;
    while ((int)BLOCKING_REGION_FD(recvmsg_blocking, &arg) == -1) {
	if (!rb_io_wait_readable(arg.fd))
	    rsock_sys_fail_path("recvmsg(2)", fptr->pathv);
    }

#if FD_PASSING_BY_MSG_CONTROL
    if (arg.msg.msg_controllen < (socklen_t)sizeof(struct cmsghdr)) {
	rb_raise(rb_eSocket,
		 "file descriptor was not passed (msg_controllen=%d smaller than sizeof(struct cmsghdr)=%d)",
		 (int)arg.msg.msg_controllen, (int)sizeof(struct cmsghdr));
    }
    if (cmsg.hdr.cmsg_level != SOL_SOCKET) {
	rb_raise(rb_eSocket,
		 "file descriptor was not passed (cmsg_level=%d, %d expected)",
		 cmsg.hdr.cmsg_level, SOL_SOCKET);
    }
    if (cmsg.hdr.cmsg_type != SCM_RIGHTS) {
	rb_raise(rb_eSocket,
		 "file descriptor was not passed (cmsg_type=%d, %d expected)",
		 cmsg.hdr.cmsg_type, SCM_RIGHTS);
    }
    if (arg.msg.msg_controllen < (socklen_t)CMSG_LEN(sizeof(int))) {
	rb_raise(rb_eSocket,
		 "file descriptor was not passed (msg_controllen=%d smaller than CMSG_LEN(sizeof(int))=%d)",
		 (int)arg.msg.msg_controllen, (int)CMSG_LEN(sizeof(int)));
    }
    if ((socklen_t)CMSG_SPACE(sizeof(int)) < arg.msg.msg_controllen) {
	rb_raise(rb_eSocket,
		 "file descriptor was not passed (msg_controllen=%d bigger than CMSG_SPACE(sizeof(int))=%d)",
		 (int)arg.msg.msg_controllen, (int)CMSG_SPACE(sizeof(int)));
    }
    if (cmsg.hdr.cmsg_len != CMSG_LEN(sizeof(int))) {
	rsock_discard_cmsg_resource(&arg.msg, 0);
	rb_raise(rb_eSocket,
		 "file descriptor was not passed (cmsg_len=%d, %d expected)",
		 (int)cmsg.hdr.cmsg_len, (int)CMSG_LEN(sizeof(int)));
    }
#else
    if (arg.msg.msg_accrightslen != sizeof(fd)) {
	rb_raise(rb_eSocket,
		 "file descriptor was not passed (accrightslen=%d, %d expected)",
		 arg.msg.msg_accrightslen, (int)sizeof(fd));
    }
#endif

#if FD_PASSING_BY_MSG_CONTROL
    memcpy(&fd, CMSG_DATA(&cmsg.hdr), sizeof(int));
#endif

    rb_update_max_fd(fd);

    if (rsock_cmsg_cloexec_state < 0)
	rsock_cmsg_cloexec_state = rsock_detect_cloexec(fd);
    if (rsock_cmsg_cloexec_state == 0 || fd <= 2)
	rb_maygvl_fd_fix_cloexec(fd);

    if (klass == Qnil)
	return INT2FIX(fd);
    else {
	ID for_fd;
	int ff_argc;
	VALUE ff_argv[2];
	CONST_ID(for_fd, "for_fd");
	ff_argc = mode == Qnil ? 1 : 2;
	ff_argv[0] = INT2FIX(fd);
	ff_argv[1] = mode;
        return rb_funcall2(klass, for_fd, ff_argc, ff_argv);
    }
}