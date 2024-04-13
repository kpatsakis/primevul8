int red_stream_send_msgfd(RedStream *stream, int fd)
{
    struct msghdr msgh = { 0, };
    struct iovec iov;
    int r;

    const size_t fd_size = 1 * sizeof(int);
    struct cmsghdr *cmsg;
    union {
        struct cmsghdr hdr;
        char data[CMSG_SPACE(fd_size)];
    } control;

    spice_return_val_if_fail(red_stream_is_plain_unix(stream), -1);

    /* set the payload */
    iov.iov_base = (char*)"@";
    iov.iov_len = 1;
    msgh.msg_iovlen = 1;
    msgh.msg_iov = &iov;

    if (fd != -1) {
        msgh.msg_control = control.data;
        msgh.msg_controllen = sizeof(control.data);
        /* CMSG_SPACE() might be larger than CMSG_LEN() as it can include some
         * padding. We set the whole control data to 0 to avoid valgrind warnings
         */
        memset(control.data, 0, sizeof(control.data));

        cmsg = CMSG_FIRSTHDR(&msgh);
        cmsg->cmsg_len = CMSG_LEN(fd_size);
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;
        memcpy(CMSG_DATA(cmsg), &fd, fd_size);
    }

    do {
        r = sendmsg(stream->socket, &msgh, MSG_NOSIGNAL);
    } while (r < 0 && (errno == EINTR || errno == EAGAIN));

    return r;
}