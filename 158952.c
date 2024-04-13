static void free_xattrmap(struct lo_data *lo)
{
    XattrMapEntry *map = lo->xattr_map_list;
    size_t i;

    if (!map) {
        return;
    }

    for (i = 0; i < lo->xattr_map_nentries; i++) {
        g_free(map[i].key);
        g_free(map[i].prepend);
    };

    g_free(map);
    lo->xattr_map_list = NULL;
    lo->xattr_map_nentries = -1;
}