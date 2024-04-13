_set_stable_privacy(NMUtilsStableType stable_type,
                    struct in6_addr * addr,
                    const char *      ifname,
                    const char *      network_id,
                    guint32           dad_counter,
                    const guint8 *    host_id,
                    gsize             host_id_len,
                    GError **         error)
{
    nm_auto_free_checksum GChecksum *sum = NULL;
    guint8                           digest[NM_UTILS_CHECKSUM_LENGTH_SHA256];
    guint32                          tmp[2];

    nm_assert(host_id_len);
    nm_assert(network_id);

    sum = g_checksum_new(G_CHECKSUM_SHA256);

    host_id_len = MIN(host_id_len, G_MAXUINT32);

    if (stable_type != NM_UTILS_STABLE_TYPE_UUID) {
        guint8 stable_type_uint8;

        nm_assert(stable_type < (NMUtilsStableType) 255);
        stable_type_uint8 = (guint8) stable_type;

        /* Preferably, we would always like to include the stable-type,
         * but for backward compatibility reasons, we cannot for UUID.
         *
         * That is no real problem and it is still impossible to
         * force a collision here, because of how the remaining
         * fields are hashed. That is, as we also hash @host_id_len
         * and the terminating '\0' of @network_id, it is unambiguously
         * possible to revert the process and deduce the @stable_type.
         */
        g_checksum_update(sum, &stable_type_uint8, sizeof(stable_type_uint8));
    }

    g_checksum_update(sum, addr->s6_addr, 8);
    g_checksum_update(sum, (const guchar *) ifname, strlen(ifname) + 1);
    g_checksum_update(sum, (const guchar *) network_id, strlen(network_id) + 1);
    tmp[0] = htonl(dad_counter);
    tmp[1] = htonl(host_id_len);
    g_checksum_update(sum, (const guchar *) tmp, sizeof(tmp));
    g_checksum_update(sum, (const guchar *) host_id, host_id_len);
    nm_utils_checksum_get_digest(sum, digest);

    while (_is_reserved_ipv6_iid(digest)) {
        g_checksum_reset(sum);
        tmp[0] = htonl(++dad_counter);
        g_checksum_update(sum, digest, sizeof(digest));
        g_checksum_update(sum, (const guchar *) &tmp[0], sizeof(tmp[0]));
        nm_utils_checksum_get_digest(sum, digest);
    }

    memcpy(addr->s6_addr + 8, &digest[0], 8);
    return TRUE;
}