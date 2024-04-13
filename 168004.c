_is_reserved_ipv6_iid(const guint8 *iid)
{
    /* https://tools.ietf.org/html/rfc5453 */
    /* https://www.iana.org/assignments/ipv6-interface-ids/ipv6-interface-ids.xml */

    /* 0000:0000:0000:0000 (Subnet-Router Anycast [RFC4291]) */
    if (memcmp(iid, &nm_ip_addr_zero.addr6.s6_addr[8], 8) == 0)
        return TRUE;

    /* 0200:5EFF:FE00:0000 - 0200:5EFF:FE00:5212 (Reserved IPv6 Interface Identifiers corresponding to the IANA Ethernet Block [RFC4291])
     * 0200:5EFF:FE00:5213                       (Proxy Mobile IPv6 [RFC6543])
     * 0200:5EFF:FE00:5214 - 0200:5EFF:FEFF:FFFF (Reserved IPv6 Interface Identifiers corresponding to the IANA Ethernet Block [RFC4291]) */
    if (memcmp(iid, (const guint8[]){0x02, 0x00, 0x5E, 0xFF, 0xFE}, 5) == 0)
        return TRUE;

    /* FDFF:FFFF:FFFF:FF80 - FDFF:FFFF:FFFF:FFFF (Reserved Subnet Anycast Addresses [RFC2526]) */
    if (memcmp(iid, (const guint8[]){0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, 7) == 0) {
        if (iid[7] & 0x80)
            return TRUE;
    }

    return FALSE;
}