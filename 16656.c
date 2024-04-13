ipf_addr_hash_add(uint32_t hash, const union ipf_addr *addr)
{
    BUILD_ASSERT_DECL(sizeof *addr % 4 == 0);
    return hash_add_bytes32(hash, (const uint32_t *) addr, sizeof *addr);
}