nm_utils_get_ipv6_interface_identifier(NMLinkType          link_type,
                                       const guint8 *      hwaddr,
                                       guint               hwaddr_len,
                                       guint               dev_id,
                                       NMUtilsIPv6IfaceId *out_iid)
{
    guint32 addr;

    g_return_val_if_fail(hwaddr != NULL, FALSE);
    g_return_val_if_fail(hwaddr_len > 0, FALSE);
    g_return_val_if_fail(out_iid != NULL, FALSE);

    out_iid->id = 0;

    switch (link_type) {
    case NM_LINK_TYPE_INFINIBAND:
        /* Use the port GUID per http://tools.ietf.org/html/rfc4391#section-8,
         * making sure to set the 'u' bit to 1.  The GUID is the lower 64 bits
         * of the IPoIB interface's hardware address.
         */
        g_return_val_if_fail(hwaddr_len == INFINIBAND_ALEN, FALSE);
        memcpy(out_iid->id_u8, hwaddr + INFINIBAND_ALEN - 8, 8);
        out_iid->id_u8[0] |= 0x02;
        return TRUE;
    case NM_LINK_TYPE_GRE:
        /* Hardware address is the network-endian IPv4 address */
        g_return_val_if_fail(hwaddr_len == 4, FALSE);
        addr              = *(guint32 *) hwaddr;
        out_iid->id_u8[0] = get_gre_eui64_u_bit(addr);
        out_iid->id_u8[1] = 0x00;
        out_iid->id_u8[2] = 0x5E;
        out_iid->id_u8[3] = 0xFE;
        memcpy(out_iid->id_u8 + 4, &addr, 4);
        return TRUE;
    case NM_LINK_TYPE_6LOWPAN:
        /* The hardware address is already 64-bit. This is the case for
         * IEEE 802.15.4 networks. */
        memcpy(out_iid->id_u8, hwaddr, sizeof(out_iid->id_u8));
        return TRUE;
    default:
        if (hwaddr_len == ETH_ALEN) {
            /* Translate 48-bit MAC address to a 64-bit Modified EUI-64.  See
             * http://tools.ietf.org/html/rfc4291#appendix-A and the Linux
             * kernel's net/ipv6/addrconf.c::ipv6_generate_eui64() function.
             */
            out_iid->id_u8[0] = hwaddr[0];
            out_iid->id_u8[1] = hwaddr[1];
            out_iid->id_u8[2] = hwaddr[2];
            if (dev_id) {
                out_iid->id_u8[3] = (dev_id >> 8) & 0xff;
                out_iid->id_u8[4] = dev_id & 0xff;
            } else {
                out_iid->id_u8[0] ^= 0x02;
                out_iid->id_u8[3] = 0xff;
                out_iid->id_u8[4] = 0xfe;
            }
            out_iid->id_u8[5] = hwaddr[3];
            out_iid->id_u8[6] = hwaddr[4];
            out_iid->id_u8[7] = hwaddr[5];
            return TRUE;
        }
        break;
    }
    return FALSE;
}