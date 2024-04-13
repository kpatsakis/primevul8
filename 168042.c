read_device_factory_paths_sort_fcn(gconstpointer a, gconstpointer b)
{
    const struct plugin_info *da = a;
    const struct plugin_info *db = b;
    time_t                    ta, tb;

    ta = MAX(da->st.st_mtime, da->st.st_ctime);
    tb = MAX(db->st.st_mtime, db->st.st_ctime);

    if (ta < tb)
        return 1;
    if (ta > tb)
        return -1;
    return 0;
}