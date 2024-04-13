ipf_print_reass_packet(const char *es, const void *pkt)
{
    static struct vlog_rate_limit rl = VLOG_RATE_LIMIT_INIT(10, 10);
    if (!VLOG_DROP_WARN(&rl)) {
        struct ds ds = DS_EMPTY_INITIALIZER;
        ds_put_hex_dump(&ds, pkt, 128, 0, false);
        VLOG_WARN("%s\n%s", es, ds_cstr(&ds));
        ds_destroy(&ds);
    }
}