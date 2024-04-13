static int cmp(const void * const a, const void * const b)
{
    return strcmp(FI_NAME((const PureFileInfo *) a),
                  FI_NAME((const PureFileInfo *) b));
}