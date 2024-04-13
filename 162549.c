void udp_cleanup(Slirp *slirp)
{
    struct socket *so, *so_next;

    for (so = slirp->udb.so_next; so != &slirp->udb; so = so_next) {
        so_next = so->so_next;
        udp_detach(slirp->udb.so_next);
    }
}