nm_utils_get_reverse_dns_domains_ip_6(const struct in6_addr *ip, guint8 plen, GPtrArray *domains)
{
    struct in6_addr addr;
    guint           nibbles, bits, entries;
    int             i, j;
    gsize           len0, len;
    char *          str, *s;

    g_return_if_fail(domains);
    g_return_if_fail(plen <= 128);

    if (!plen)
        return;

    memcpy(&addr, ip, sizeof(struct in6_addr));
    nm_utils_ip6_address_clear_host_address(&addr, NULL, plen);

    /* Number of nibbles to include in domains */
    nibbles = (plen - 1) / 4 + 1;
    /* Prefix length in nibble */
    bits = plen - ((plen - 1) / 4 * 4);
    /* Number of domains */
    entries = 1 << (4 - bits);

    len0 = NM_STRLEN("ip6.arpa") + (2 * nibbles) + 1;

#define N_SHIFT(x) ((x) % 2 ? 0 : 4)

    for (i = 0; i < entries; i++) {
        len = len0;
        str = s = g_malloc(len);

        for (j = nibbles - 1; j >= 0; j--)
            nm_utils_strbuf_append(&s, &len, "%x.", (addr.s6_addr[j / 2] >> N_SHIFT(j)) & 0xf);
        nm_utils_strbuf_append_str(&s, &len, "ip6.arpa");

        g_ptr_array_add(domains, str);

        addr.s6_addr[(nibbles - 1) / 2] += 1 << N_SHIFT(nibbles - 1);
    }

#undef N_SHIFT
}