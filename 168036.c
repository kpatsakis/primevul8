nm_utils_host_id_get(const guint8 **out_host_id, gsize *out_host_id_len)
{
    const HostIdData *host_id;

    host_id          = _host_id_get();
    *out_host_id     = host_id->host_id;
    *out_host_id_len = host_id->host_id_len;
    return host_id->is_good;
}