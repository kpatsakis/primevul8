static int cmp_r(const void * const a, const void * const b)
{
    return strcmp(FI_NAME((const PureFileInfo *) b),
                  FI_NAME((const PureFileInfo *) a));
}