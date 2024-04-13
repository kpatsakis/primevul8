ipf_list_key_eq(const struct ipf_list_key *key1,
                const struct ipf_list_key *key2)
    /* OVS_REQUIRES(ipf_lock) */
{
    if (!memcmp(&key1->src_addr, &key2->src_addr, sizeof key1->src_addr) &&
        !memcmp(&key1->dst_addr, &key2->dst_addr, sizeof key1->dst_addr) &&
        key1->dl_type == key2->dl_type &&
        key1->ip_id == key2->ip_id &&
        key1->zone == key2->zone &&
        key1->nw_proto == key2->nw_proto &&
        key1->recirc_id == key2->recirc_id) {
        return true;
    }
    return false;
}