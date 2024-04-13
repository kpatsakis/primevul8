vg_sock_fd_write(int sock, const void *buf, ssize_t buflen, int fd)
{
    ssize_t ret;
    struct iovec iov = {
        .iov_base = (void *)buf,
        .iov_len = buflen,
    };
    struct msghdr msg = {
        .msg_iov = &iov,
        .msg_iovlen = 1,
    };
    union {
        struct cmsghdr cmsghdr;
        char control[CMSG_SPACE(sizeof(int))];
    } cmsgu;
    struct cmsghdr *cmsg;

    if (fd != -1) {
        msg.msg_control = cmsgu.control;
        msg.msg_controllen = sizeof(cmsgu.control);

        cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_len = CMSG_LEN(sizeof(int));
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;

        *((int *)CMSG_DATA(cmsg)) = fd;
    }

    do {
        ret = sendmsg(sock, &msg, 0);
    } while (ret == -1 && (errno == EINTR || errno == EAGAIN));

    g_warn_if_fail(ret == buflen);
    return ret;
}