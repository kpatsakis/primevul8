static inline int media_present(IDEState *s)
{
    return !s->tray_open && s->nb_sectors > 0;
}