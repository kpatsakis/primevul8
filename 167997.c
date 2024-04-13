nm_utils_dhcp_client_id_mac(int arp_type, const guint8 *hwaddr, gsize hwaddr_len)
{
    guint8 *     client_id_buf;
    const guint8 hwaddr_type = arp_type;

    if (!nm_utils_arp_type_get_hwaddr_relevant_part(arp_type, &hwaddr, &hwaddr_len))
        g_return_val_if_reached(NULL);

    client_id_buf    = g_malloc(hwaddr_len + 1);
    client_id_buf[0] = hwaddr_type;
    memcpy(&client_id_buf[1], hwaddr, hwaddr_len);
    return g_bytes_new_take(client_id_buf, hwaddr_len + 1);
}