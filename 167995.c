nm_utils_arp_type_detect_from_hwaddrlen(gsize hwaddr_len)
{
    switch (hwaddr_len) {
    case ETH_ALEN:
        return ARPHRD_ETHER;
    case INFINIBAND_ALEN:
        return ARPHRD_INFINIBAND;
    default:
        /* Note: if you ever support anything but ethernet and infiniband,
         * make sure to look at all callers. They assert that it's one of
         * these two. */
        return -EINVAL;
    }
}