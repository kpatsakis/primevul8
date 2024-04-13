_host_id_get(void)
{
    static const HostIdData *volatile host_id_static;
    const HostIdData *host_id;

again:
    host_id = g_atomic_pointer_get(&host_id_static);
    if (G_UNLIKELY(!host_id)) {
        static HostIdData host_id_data;
        static gsize      init_value = 0;

        if (!g_once_init_enter(&init_value))
            goto again;

        host_id_data.is_good = _host_id_read(&host_id_data.host_id, &host_id_data.host_id_len);

        host_id_data.timestamp_is_good = _host_id_read_timestamp(host_id_data.is_good,
                                                                 host_id_data.host_id,
                                                                 host_id_data.host_id_len,
                                                                 &host_id_data.timestamp_ns);
        if (!host_id_data.timestamp_is_good && host_id_data.is_good)
            nm_log_warn(LOGD_CORE, "secret-key: failure reading host timestamp (use fake one)");

        host_id = &host_id_data;
        g_atomic_pointer_set(&host_id_static, host_id);
        g_once_init_leave(&init_value, 1);
    }

    return host_id;
}