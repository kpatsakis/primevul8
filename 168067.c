nm_utils_arp_type_get_hwaddr_relevant_part(int arp_type, const guint8 **hwaddr, gsize *hwaddr_len)
{
    g_return_val_if_fail(hwaddr && hwaddr_len
                             && nm_utils_arp_type_validate_hwaddr(arp_type, *hwaddr, *hwaddr_len),
                         FALSE);

    /* for infiniband, we only consider the last 8 bytes. */
    if (arp_type == ARPHRD_INFINIBAND) {
        *hwaddr += (INFINIBAND_ALEN - 8);
        *hwaddr_len = 8;
    }

    return TRUE;
}