_machine_id_get(gboolean allow_fake)
{
    static const UuidData *volatile p_uuid_data;
    const UuidData *d;

again:
    d = g_atomic_pointer_get(&p_uuid_data);
    if (G_UNLIKELY(!d)) {
        static gsize    lock;
        static UuidData uuid_data;
        gs_free char *  content   = NULL;
        gboolean        is_fake   = TRUE;
        const char *    fake_type = NULL;
        NMUuid          uuid;

        /* Get the machine ID from /etc/machine-id; it's always in /etc no matter
         * where our configured SYSCONFDIR is.  Alternatively, it might be in
         * LOCALSTATEDIR /lib/dbus/machine-id.
         */
        if (nm_utils_file_get_contents(-1,
                                       "/etc/machine-id",
                                       100 * 1024,
                                       0,
                                       &content,
                                       NULL,
                                       NULL,
                                       NULL)
            || nm_utils_file_get_contents(-1,
                                          LOCALSTATEDIR "/lib/dbus/machine-id",
                                          100 * 1024,
                                          0,
                                          &content,
                                          NULL,
                                          NULL,
                                          NULL)) {
            g_strstrip(content);
            if (nm_utils_hexstr2bin_full(content,
                                         FALSE,
                                         FALSE,
                                         FALSE,
                                         NULL,
                                         16,
                                         (guint8 *) &uuid,
                                         sizeof(uuid),
                                         NULL)) {
                if (!nm_utils_uuid_is_null(&uuid)) {
                    /* an all-zero machine-id is not valid. */
                    is_fake = FALSE;
                }
            }
        }

        if (is_fake) {
            const guint8 *seed_bin;
            const char *  hash_seed;
            gsize         seed_len;

            if (!allow_fake) {
                /* we don't allow generating (and memorizing) a fake key.
                 * Signal that no valid machine-id exists. */
                return NULL;
            }

            if (nm_utils_host_id_get(&seed_bin, &seed_len)) {
                /* We have no valid machine-id but we have a valid secrey_key.
                 * Generate a fake machine ID by hashing the secret-key. The secret_key
                 * is commonly persisted, so it should be stable across reboots (despite
                 * having a broken system without proper machine-id).
                 *
                 * Note that we access the host-id here, which is based on secret_key.
                 * Also not that the secret_key may be generated based on the machine-id,
                 * so we have to be careful that they don't depend on each other (and
                 * no infinite recursion happens. This is done correctly, because the secret-key
                 * will call _machine_id_get(FALSE), so it won't allow accessing a fake
                 * machine-id, thus avoiding the problem. */
                fake_type = "secret-key";
                hash_seed = "ab085f06-b629-46d1-a553-84eeba5683b6";
            } else {
                /* the secret-key is not valid/persistent either. That happens when we fail
                 * to read/write the secret-key to disk. Fallback to boot-id. The boot-id
                 * itself may be fake and randomly generated ad-hoc, but that is as best
                 * as it gets.  */
                seed_bin  = (const guint8 *) nm_utils_boot_id_bin();
                seed_len  = sizeof(NMUuid);
                fake_type = "boot-id";
                hash_seed = "7ff0c8f5-5399-4901-ab63-61bf594abe8b";
            }

            /* the fake machine-id is based on secret-key/boot-id, but we hash it
             * again, so that they are not literally the same. */
            nm_utils_uuid_generate_from_string_bin(&uuid,
                                                   (const char *) seed_bin,
                                                   seed_len,
                                                   NM_UTILS_UUID_TYPE_VERSION5,
                                                   (gpointer) hash_seed);
        }

        if (!g_once_init_enter(&lock))
            goto again;

        d = _uuid_data_init(&uuid_data, TRUE, is_fake, &uuid);
        g_atomic_pointer_set(&p_uuid_data, d);
        g_once_init_leave(&lock, 1);

        if (is_fake) {
            nm_log_err(LOGD_CORE,
                       "/etc/machine-id: no valid machine-id. Use fake one based on %s: %s",
                       fake_type,
                       d->str);
        } else
            nm_log_dbg(LOGD_CORE, "/etc/machine-id: %s", d->str);
    }

    return d;
}