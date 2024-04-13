nm_utils_dhcp_client_id_systemd_node_specific_full(guint32       iaid,
                                                   const guint8 *machine_id,
                                                   gsize         machine_id_len)
{
    const guint16 DUID_TYPE_EN = 2;
    const guint32 SYSTEMD_PEN  = 43793;
    struct _nm_packed {
        guint8  type;
        guint32 iaid;
        struct _nm_packed {
            guint16 type;
            union {
                struct _nm_packed {
                    /* DUID_TYPE_EN */
                    guint32 pen;
                    uint8_t id[8];
                } en;
            };
        } duid;
    } * client_id;
    guint64 u64;

    g_return_val_if_fail(machine_id, NULL);
    g_return_val_if_fail(machine_id_len > 0, NULL);

    client_id = g_malloc(sizeof(*client_id));

    client_id->type = 255;
    unaligned_write_be32(&client_id->iaid, iaid);
    unaligned_write_be16(&client_id->duid.type, DUID_TYPE_EN);
    unaligned_write_be32(&client_id->duid.en.pen, SYSTEMD_PEN);

    u64 = htole64(c_siphash_hash(HASH_KEY, machine_id, machine_id_len));
    memcpy(client_id->duid.en.id, &u64, sizeof(client_id->duid.en.id));

    G_STATIC_ASSERT_EXPR(sizeof(*client_id) == 19);
    return g_bytes_new_take(client_id, 19);
}