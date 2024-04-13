nm_utils_ipv6_addr_set_stable_privacy(NMUtilsStableType stable_type,
                                      struct in6_addr * addr,
                                      const char *      ifname,
                                      const char *      network_id,
                                      guint32           dad_counter,
                                      GError **         error)
{
    const guint8 *host_id;
    gsize         host_id_len;

    g_return_val_if_fail(network_id, FALSE);

    if (dad_counter >= RFC7217_IDGEN_RETRIES) {
        g_set_error_literal(error,
                            NM_UTILS_ERROR,
                            NM_UTILS_ERROR_UNKNOWN,
                            "Too many DAD collisions");
        return FALSE;
    }

    nm_utils_host_id_get(&host_id, &host_id_len);

    return _set_stable_privacy(stable_type,
                               addr,
                               ifname,
                               network_id,
                               dad_counter,
                               host_id,
                               host_id_len,
                               error);
}