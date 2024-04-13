get_gre_eui64_u_bit(guint32 addr)
{
    static const struct {
        guint32 mask;
        guint32 result;
    } items[] = {
        {0xff000000}, {0x7f000000},       /* IPv4 loopback */
        {0xf0000000}, {0xe0000000},       /* IPv4 multicast */
        {0xffffff00}, {0xe0000000},       /* IPv4 local multicast */
        {0xffffffff}, {INADDR_BROADCAST}, /* limited broadcast */
        {0xff000000}, {0x00000000},       /* zero net */
        {0xff000000}, {0x0a000000},       /* private 10 (RFC3330) */
        {0xfff00000}, {0xac100000},       /* private 172 */
        {0xffff0000}, {0xc0a80000},       /* private 192 */
        {0xffff0000}, {0xa9fe0000},       /* IPv4 link-local */
        {0xffffff00}, {0xc0586300},       /* anycast 6-to-4 */
        {0xffffff00}, {0xc0000200},       /* test 192 */
        {0xfffe0000}, {0xc6120000},       /* test 198 */
    };
    guint i;

    for (i = 0; i < G_N_ELEMENTS(items); i++) {
        if ((addr & htonl(items[i].mask)) == htonl(items[i].result))
            return 0x00; /* "local" scope */
    }
    return 0x02; /* "universal" scope */
}