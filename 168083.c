nm_utils_generate_duid_ll(int arp_type, const guint8 *hwaddr, gsize hwaddr_len)
{
    guint8 *      arr;
    const guint16 duid_type = htons(3);
    const guint16 hw_type   = htons(arp_type);

    if (!nm_utils_arp_type_get_hwaddr_relevant_part(arp_type, &hwaddr, &hwaddr_len))
        nm_assert_not_reached();

    arr = g_new(guint8, (2u + 2u) + hwaddr_len);

    memcpy(&arr[0], &duid_type, 2);
    memcpy(&arr[2], &hw_type, 2);
    memcpy(&arr[4], hwaddr, hwaddr_len);

    return g_bytes_new_take(arr, (2u + 2u) + hwaddr_len);
}