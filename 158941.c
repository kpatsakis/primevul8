static void add_xattrmap_entry(struct lo_data *lo,
                               const XattrMapEntry *new_entry)
{
    XattrMapEntry *res = g_realloc_n(lo->xattr_map_list,
                                     lo->xattr_map_nentries + 1,
                                     sizeof(XattrMapEntry));
    res[lo->xattr_map_nentries++] = *new_entry;

    lo->xattr_map_list = res;
}