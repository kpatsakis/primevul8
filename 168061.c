nm_utils_hw_addr_gen_stable_eth(NMUtilsStableType stable_type,
                                const char *      stable_id,
                                const char *      ifname,
                                const char *      current_mac_address,
                                const char *      generate_mac_address_mask)
{
    const guint8 *host_id;
    gsize         host_id_len;

    g_return_val_if_fail(stable_id, NULL);

    nm_utils_host_id_get(&host_id, &host_id_len);

    return _hw_addr_gen_stable_eth(stable_type,
                                   stable_id,
                                   host_id,
                                   host_id_len,
                                   ifname,
                                   current_mac_address,
                                   generate_mac_address_mask);
}