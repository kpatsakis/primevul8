nm_ether_addr_is_valid(const NMEtherAddr *addr)
{
    static const guint8 invalid_addr[][ETH_ALEN] = {
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x44, 0x44, 0x44, 0x44, 0x44, 0x44},
        {0x00, 0x30, 0xb4, 0x00, 0x00, 0x00}, /* prism54 dummy MAC */
    };
    int i;

    if (!addr)
        return FALSE;

    /* Check for multicast address */
    if (addr->ether_addr_octet[0] & 0x01u)
        return FALSE;

    for (i = 0; i < (int) G_N_ELEMENTS(invalid_addr); i++) {
        if (memcmp(addr, invalid_addr[i], ETH_ALEN) == 0)
            return FALSE;
    }

    return TRUE;
}