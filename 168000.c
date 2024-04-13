nm_utils_hw_addr_gen_stable_eth_impl(NMUtilsStableType stable_type,
                                     const char *      stable_id,
                                     const guint8 *    host_id,
                                     gsize             host_id_len,
                                     const char *      ifname,
                                     const char *      current_mac_address,
                                     const char *      generate_mac_address_mask)
{
    return _hw_addr_gen_stable_eth(stable_type,
                                   stable_id,
                                   host_id,
                                   host_id_len,
                                   ifname,
                                   current_mac_address,
                                   generate_mac_address_mask);
}