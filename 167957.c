static inline int media_is_cd(IDEState *s)
{
    return (media_present(s) && s->nb_sectors <= CD_MAX_SECTORS);
}