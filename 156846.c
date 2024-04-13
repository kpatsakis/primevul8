static inline bool media_is_cd(SCSIDiskState *s)
{
    uint64_t nb_sectors;
    if (s->qdev.type != TYPE_ROM) {
        return false;
    }
    if (!blk_is_available(s->qdev.conf.blk)) {
        return false;
    }
    blk_get_geometry(s->qdev.conf.blk, &nb_sectors);
    return nb_sectors <= CD_MAX_SECTORS;
}