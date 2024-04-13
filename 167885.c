vg_sock_fd_read(int sock, void *buf, ssize_t buflen)
{
    int ret;

    do {
        ret = read(sock, buf, buflen);
    } while (ret < 0 && (errno == EINTR || errno == EAGAIN));

    g_warn_if_fail(ret == buflen);
    return ret;
}