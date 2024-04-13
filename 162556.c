int udp_attach(struct socket *so, unsigned short af)
{
    so->s = slirp_socket(af, SOCK_DGRAM, 0);
    if (so->s != -1) {
        if (slirp_bind_outbound(so, af) != 0) {
            // bind failed - close socket
            closesocket(so->s);
            so->s = -1;
            return -1;
        }

#ifdef __linux__
        {
            int opt = 1;
            switch (af) {
            case AF_INET:
                setsockopt(so->s, IPPROTO_IP, IP_RECVERR, &opt, sizeof(opt));
                break;
            case AF_INET6:
                setsockopt(so->s, IPPROTO_IPV6, IPV6_RECVERR, &opt, sizeof(opt));
                break;
            default:
                g_assert_not_reached();
            }
        }
#endif

        so->so_expire = curtime + SO_EXPIRE;
        insque(so, &so->slirp->udb);
    }
    so->slirp->cb->register_poll_fd(so->s, so->slirp->opaque);
    return (so->s);
}