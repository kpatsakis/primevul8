int ifd_size_t_cmp(const void *a, const void *b)
{
    if(!a || !b) return 0;
    const ifd_size_t *ai = (ifd_size_t*)a;
    const ifd_size_t *bi = (ifd_size_t*)b;
    return bi->databits > ai->databits ? 1 : (bi->databits < ai->databits? -1 : 0);
}