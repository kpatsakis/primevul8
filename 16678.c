ipf_list_key_hash(const struct ipf_list_key *key, uint32_t basis)
{
    uint32_t hsrc, hdst, hash;
    hsrc = hdst = basis;
    hsrc = ipf_addr_hash_add(hsrc, &key->src_addr);
    hdst = ipf_addr_hash_add(hdst, &key->dst_addr);
    hash = hsrc ^ hdst;

    /* Hash the rest of the key. */
    return hash_words((uint32_t *) (&key->dst_addr + 1),
                      (uint32_t *) (key + 1) -
                      (uint32_t *) (&key->dst_addr + 1),
                      hash);
}