struct socket *udp_listen(Slirp *slirp, uint32_t haddr, unsigned hport,
                          uint32_t laddr, unsigned lport, int flags)
{
    struct sockaddr_in hsa, lsa;

    memset(&hsa, 0, sizeof(hsa));
    hsa.sin_family = AF_INET;
    hsa.sin_addr.s_addr = haddr;
    hsa.sin_port = hport;

    memset(&lsa, 0, sizeof(lsa));
    lsa.sin_family = AF_INET;
    lsa.sin_addr.s_addr = laddr;
    lsa.sin_port = lport;

    return udpx_listen(slirp, (const struct sockaddr *) &hsa, sizeof(hsa), (struct sockaddr *) &lsa, sizeof(lsa), flags);
}