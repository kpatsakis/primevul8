nm_utils_generate_duid_from_machine_id(void)
{
    static GBytes *volatile global_duid = NULL;
    GBytes *p;

again:
    p = g_atomic_pointer_get(&global_duid);
    if (G_UNLIKELY(!p)) {
        nm_auto_free_checksum GChecksum *sum = NULL;
        const NMUuid *                   machine_id;
        union {
            guint8 sha256[NM_UTILS_CHECKSUM_LENGTH_SHA256];
            NMUuid uuid;
        } digest;

        machine_id = nm_utils_machine_id_bin();

        /* Hash the machine ID so it's not leaked to the network.
         *
         * Optimally, we would choose an use case specific seed, but for historic
         * reasons we didn't. */
        sum = g_checksum_new(G_CHECKSUM_SHA256);
        g_checksum_update(sum, (const guchar *) machine_id, sizeof(*machine_id));
        nm_utils_checksum_get_digest(sum, digest.sha256);

        G_STATIC_ASSERT_EXPR(sizeof(digest.sha256) > sizeof(digest.uuid));
        p = nm_utils_generate_duid_uuid(&digest.uuid);

        if (!g_atomic_pointer_compare_and_exchange(&global_duid, NULL, p)) {
            g_bytes_unref(p);
            goto again;
        }
    }

    return g_bytes_ref(p);
}