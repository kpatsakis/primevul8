nm_utils_ipv6_addr_set_stable_privacy_impl(NMUtilsStableType stable_type,
                                           struct in6_addr * addr,
                                           const char *      ifname,
                                           const char *      network_id,
                                           guint32           dad_counter,
                                           guint8 *          host_id,
                                           gsize             host_id_len,
                                           GError **         error)
{
    return _set_stable_privacy(stable_type,
                               addr,
                               ifname,
                               network_id,
                               dad_counter,
                               host_id,
                               host_id_len,
                               error);
}