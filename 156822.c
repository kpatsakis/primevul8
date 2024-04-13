static void scsi_disk_apply_mode_select(SCSIDiskState *s, int page, uint8_t *p)
{
    switch (page) {
    case MODE_PAGE_CACHING:
        blk_set_enable_write_cache(s->qdev.conf.blk, (p[0] & 4) != 0);
        break;

    default:
        break;
    }
}