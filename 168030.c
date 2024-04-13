nm_utils_dhcp_client_id_duid(guint32 iaid, const guint8 *duid, gsize duid_len)
{
    struct _nm_packed {
        guint8  type;
        guint32 iaid;
        guint8  duid[];
    } * client_id;
    gsize total_size;

    /* the @duid must include the 16 bit duid-type and the data (of max 128 bytes). */
    g_return_val_if_fail(duid_len > 2 && duid_len < 128 + 2, NULL);
    g_return_val_if_fail(duid, NULL);

    total_size = sizeof(*client_id) + duid_len;

    client_id = g_malloc(total_size);

    client_id->type = 255;
    unaligned_write_be32(&client_id->iaid, iaid);
    memcpy(client_id->duid, duid, duid_len);
    return g_bytes_new_take(client_id, total_size);
}