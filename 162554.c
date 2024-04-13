void udp_detach(struct socket *so)
{
    so->slirp->cb->unregister_poll_fd(so->s, so->slirp->opaque);
    closesocket(so->s);
    sofree(so);
}