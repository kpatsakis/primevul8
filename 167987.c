nm_utils_get_reverse_dns_domains_ip_4(guint32 addr, guint8 plen, GPtrArray *domains)
{
    guint32 ip, ip2, mask;
    guchar *p;
    guint   octets;
    guint   i;
    gsize   len0, len;
    char *  str, *s;

    g_return_if_fail(domains);
    g_return_if_fail(plen <= 32);

    if (!plen)
        return;

    octets = (plen - 1) / 8 + 1;
    ip     = ntohl(addr);
    mask   = 0xFFFFFFFF << (32 - plen);
    ip &= mask;
    ip2 = ip;

    len0 = NM_STRLEN("in-addr.arpa") + (4 * octets) + 1;
    while ((ip2 & mask) == ip) {
        addr = htonl(ip2);
        p    = (guchar *) &addr;

        len = len0;
        str = s = g_malloc(len);
        for (i = octets; i > 0; i--)
            nm_utils_strbuf_append(&s, &len, "%u.", p[i - 1] & 0xff);
        nm_utils_strbuf_append_str(&s, &len, "in-addr.arpa");

        g_ptr_array_add(domains, str);

        ip2 += 1 << ((32 - plen) & ~7);
    }
}