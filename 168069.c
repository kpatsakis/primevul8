_hw_addr_eth_complete(struct ether_addr *addr,
                      const char *       current_mac_address,
                      const char *       generate_mac_address_mask)
{
    struct ether_addr  mask;
    struct ether_addr  oui;
    struct ether_addr *ouis;
    gsize              ouis_len;
    guint              i;

    /* the second LSB of the first octet means
     * "globally unique, OUI enforced, BIA (burned-in-address)"
     * vs. "locally-administered". By default, set it to
     * generate locally-administered addresses.
     *
     * Maybe be overwritten by a mask below. */
    addr->ether_addr_octet[0] |= 2;

    if (!generate_mac_address_mask || !*generate_mac_address_mask)
        goto out;
    if (!_nm_utils_generate_mac_address_mask_parse(generate_mac_address_mask,
                                                   &mask,
                                                   &ouis,
                                                   &ouis_len,
                                                   NULL))
        goto out;

    nm_assert((ouis == NULL) ^ (ouis_len != 0));
    if (ouis) {
        /* g_random_int() is good enough here. It uses a static GRand instance
         * that is seeded from /dev/urandom. */
        oui = ouis[g_random_int() % ouis_len];
        g_free(ouis);
    } else {
        if (!nm_utils_hwaddr_aton(current_mac_address, &oui, ETH_ALEN))
            goto out;
    }

    for (i = 0; i < ETH_ALEN; i++) {
        const guint8 a = addr->ether_addr_octet[i];
        const guint8 o = oui.ether_addr_octet[i];
        const guint8 m = mask.ether_addr_octet[i];

        addr->ether_addr_octet[i] = (a & ~m) | (o & m);
    }

out:
    /* The LSB of the first octet must always be cleared,
     * it means Unicast vs. Multicast */
    addr->ether_addr_octet[0] &= ~1;
}