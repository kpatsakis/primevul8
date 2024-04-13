static int cmp_S(const void * const a, const void * const b)
{
    if (((const PureFileInfo *) a)->size < ((const PureFileInfo *) b)->size) {
        return 1;
    }
    if (((const PureFileInfo *) a)->size > ((const PureFileInfo *) b)->size) {
        return -1;
    }
    return 0;
}