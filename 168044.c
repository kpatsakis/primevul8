nm_utils_generate_duid_llt(int arp_type, const guint8 *hwaddr, gsize hwaddr_len, gint64 time)
{
    guint8 *      arr;
    const guint16 duid_type = htons(1);
    const guint16 hw_type   = htons(arp_type);
    const guint32 duid_time = htonl(NM_MAX(0, time - NM_UTILS_EPOCH_DATETIME_200001010000));

    if (!nm_utils_arp_type_get_hwaddr_relevant_part(arp_type, &hwaddr, &hwaddr_len))
        nm_assert_not_reached();

    arr = g_new(guint8, (2u + 2u + 4u) + hwaddr_len);

    memcpy(&arr[0], &duid_type, 2);
    memcpy(&arr[2], &hw_type, 2);
    memcpy(&arr[4], &duid_time, 4);
    memcpy(&arr[8], hwaddr, hwaddr_len);

    return g_bytes_new_take(arr, (2u + 2u + 4u) + hwaddr_len);
}