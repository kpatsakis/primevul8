nm_utils_arp_type_validate_hwaddr(int arp_type, const guint8 *hwaddr, gsize hwaddr_len)
{
    if (!hwaddr)
        return FALSE;

    if (arp_type == ARPHRD_ETHER) {
        G_STATIC_ASSERT(ARPHRD_ETHER >= 0 && ARPHRD_ETHER <= 0xFF);
        if (hwaddr_len != ETH_ALEN)
            return FALSE;
    } else if (arp_type == ARPHRD_INFINIBAND) {
        G_STATIC_ASSERT(ARPHRD_INFINIBAND >= 0 && ARPHRD_INFINIBAND <= 0xFF);
        if (hwaddr_len != INFINIBAND_ALEN)
            return FALSE;
    } else
        return FALSE;

    nm_assert(arp_type == nm_utils_arp_type_detect_from_hwaddrlen(hwaddr_len));
    return TRUE;
}