static uint8_t udp_tos(struct socket *so)
{
    int i = 0;

    while (udptos[i].tos) {
        if ((udptos[i].fport && ntohs(so->so_fport) == udptos[i].fport) ||
            (udptos[i].lport && ntohs(so->so_lport) == udptos[i].lport)) {
            if (so->slirp->enable_emu)
                so->so_emu = udptos[i].emu;
            return udptos[i].tos;
        }
        i++;
    }

    return 0;
}