nm_utils_dhcp_client_id_systemd_node_specific(guint32 iaid)
{
    return nm_utils_dhcp_client_id_systemd_node_specific_full(
        iaid,
        (const guint8 *) nm_utils_machine_id_bin(),
        sizeof(NMUuid));
}