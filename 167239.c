static int cmp_t(const void * const a, const void * const b)
{
    if (((const PureFileInfo *) a)->mtime < ((const PureFileInfo *) b)->mtime) {
        return 1;
    }
    if (((const PureFileInfo *) a)->mtime > ((const PureFileInfo *) b)->mtime) {
        return -1;
    }
    return 0;
}