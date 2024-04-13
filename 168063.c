_uuid_data_init(UuidData *uuid_data, gboolean packed, gboolean is_fake, const NMUuid *uuid)
{
    nm_assert(uuid_data);
    nm_assert(uuid);

    uuid_data->bin           = *uuid;
    uuid_data->_nul_sentinel = '\0';
    uuid_data->is_fake       = is_fake;
    if (packed) {
        G_STATIC_ASSERT_EXPR(sizeof(uuid_data->str) >= (sizeof(*uuid) * 2 + 1));
        nm_utils_bin2hexstr_full(uuid, sizeof(*uuid), '\0', FALSE, uuid_data->str);
    } else {
        G_STATIC_ASSERT_EXPR(sizeof(uuid_data->str) >= 37);
        _nm_utils_uuid_unparse(uuid, uuid_data->str);
    }
    return uuid_data;
}