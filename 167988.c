nm_ether_addr_is_valid_str(const char *str)
{
    NMEtherAddr addr_bin;

    if (!str)
        return FALSE;

    if (!nm_utils_hwaddr_aton(str, &addr_bin, ETH_ALEN))
        return FALSE;

    return nm_ether_addr_is_valid(&addr_bin);
}