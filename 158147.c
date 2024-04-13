lldpd_af_from_lldp_proto(int proto)
{
    switch (proto) {
    case LLDP_MGMT_ADDR_IP4:
        return LLDPD_AF_IPV4;
    case LLDP_MGMT_ADDR_IP6:
        return LLDPD_AF_IPV6;
    default:
        return LLDPD_AF_UNSPEC;
    }
}