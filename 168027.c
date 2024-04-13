_boot_id_get(void)
{
    static const UuidData *volatile p_boot_id;
    const UuidData *d;

again:
    d = g_atomic_pointer_get(&p_boot_id);
    if (G_UNLIKELY(!d)) {
        static gsize    lock;
        static UuidData boot_id;
        gs_free char *  contents = NULL;
        NMUuid          uuid;
        gboolean        is_fake = FALSE;

        nm_utils_file_get_contents(-1,
                                   "/proc/sys/kernel/random/boot_id",
                                   0,
                                   NM_UTILS_FILE_GET_CONTENTS_FLAG_NONE,
                                   &contents,
                                   NULL,
                                   NULL,
                                   NULL);
        if (!contents || !_nm_utils_uuid_parse(nm_strstrip(contents), &uuid)) {
            /* generate a random UUID instead. */
            is_fake = TRUE;
            _nm_utils_uuid_generate_random(&uuid);
        }

        if (!g_once_init_enter(&lock))
            goto again;

        d = _uuid_data_init(&boot_id, FALSE, is_fake, &uuid);
        g_atomic_pointer_set(&p_boot_id, d);
        g_once_init_leave(&lock, 1);
    }

    return d;
}