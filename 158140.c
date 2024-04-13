lldpd_af_to_lldp_proto(int af)
{
    switch (af) {
    case LLDPD_AF_IPV4:
        return LLDP_MGMT_ADDR_IP4;
    case LLDPD_AF_IPV6:
        return LLDP_MGMT_ADDR_IP6;
    default:
        return LLDP_MGMT_ADDR_NONE;
    }
}