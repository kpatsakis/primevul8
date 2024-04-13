void udp_init(Slirp *slirp)
{
    slirp->udb.so_next = slirp->udb.so_prev = &slirp->udb;
    slirp->udp_last_so = &slirp->udb;
}