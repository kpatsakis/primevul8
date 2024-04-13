_hw_addr_gen_stable_eth(NMUtilsStableType stable_type,
                        const char *      stable_id,
                        const guint8 *    host_id,
                        gsize             host_id_len,
                        const char *      ifname,
                        const char *      current_mac_address,
                        const char *      generate_mac_address_mask)
{
    nm_auto_free_checksum GChecksum *sum = NULL;
    guint32                          tmp;
    guint8                           digest[NM_UTILS_CHECKSUM_LENGTH_SHA256];
    struct ether_addr                bin_addr;
    guint8                           stable_type_uint8;

    nm_assert(stable_id);
    nm_assert(host_id);

    sum = g_checksum_new(G_CHECKSUM_SHA256);

    host_id_len = MIN(host_id_len, G_MAXUINT32);

    nm_assert(stable_type < (NMUtilsStableType) 255);
    stable_type_uint8 = stable_type;
    g_checksum_update(sum, (const guchar *) &stable_type_uint8, sizeof(stable_type_uint8));

    tmp = htonl((guint32) host_id_len);
    g_checksum_update(sum, (const guchar *) &tmp, sizeof(tmp));
    g_checksum_update(sum, (const guchar *) host_id, host_id_len);
    g_checksum_update(sum, (const guchar *) (ifname ?: ""), ifname ? (strlen(ifname) + 1) : 1);
    g_checksum_update(sum, (const guchar *) stable_id, strlen(stable_id) + 1);

    nm_utils_checksum_get_digest(sum, digest);

    memcpy(&bin_addr, digest, ETH_ALEN);
    _hw_addr_eth_complete(&bin_addr, current_mac_address, generate_mac_address_mask);
    return nm_utils_hwaddr_ntoa(&bin_addr, ETH_ALEN);
}