nm_utils_hw_addr_gen_random_eth(const char *current_mac_address,
                                const char *generate_mac_address_mask)
{
    struct ether_addr bin_addr;

    nm_utils_random_bytes(&bin_addr, ETH_ALEN);
    _hw_addr_eth_complete(&bin_addr, current_mac_address, generate_mac_address_mask);
    return nm_utils_hwaddr_ntoa(&bin_addr, ETH_ALEN);
}