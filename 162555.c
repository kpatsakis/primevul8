struct socket *udpx_listen(Slirp *slirp,
                           const struct sockaddr *haddr, socklen_t haddrlen,
                           const struct sockaddr *laddr, socklen_t laddrlen,
                           int flags)
{
    struct socket *so;
    socklen_t addrlen;
    int save_errno;

    so = socreate(slirp);
    so->s = slirp_socket(haddr->sa_family, SOCK_DGRAM, 0);
    if (so->s < 0) {
        save_errno = errno;
        sofree(so);
        errno = save_errno;
        return NULL;
    }
    if (haddr->sa_family == AF_INET6)
        slirp_socket_set_v6only(so->s, (flags & SS_HOSTFWD_V6ONLY) != 0);
    so->so_expire = curtime + SO_EXPIRE;
    insque(so, &slirp->udb);

    if (bind(so->s, haddr, haddrlen) < 0) {
        save_errno = errno;
        udp_detach(so);
        errno = save_errno;
        return NULL;
    }
    slirp_socket_set_fast_reuse(so->s);

    addrlen = sizeof(so->fhost);
    getsockname(so->s, &so->fhost.sa, &addrlen);
    sotranslate_accept(so);

    sockaddr_copy(&so->lhost.sa, sizeof(so->lhost), laddr, laddrlen);

    if (flags != SS_FACCEPTONCE)
        so->so_expire = 0;
    so->so_state &= SS_PERSISTENT_MASK;
    so->so_state |= SS_ISFCONNECTED | flags;

    return so;
}