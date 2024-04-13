nm_utils_proc_cmdline_split(void)
{
    static const char *const *volatile proc_cmdline_cached = NULL;
    const char *const *proc_cmdline;

again:
    proc_cmdline = g_atomic_pointer_get(&proc_cmdline_cached);
    if (G_UNLIKELY(!proc_cmdline)) {
        gs_strfreev char **split = NULL;

        split = nm_utils_strsplit_quoted(nm_utils_proc_cmdline());
        if (!g_atomic_pointer_compare_and_exchange(&proc_cmdline_cached, NULL, (gpointer) split))
            goto again;

        proc_cmdline = (const char *const *) g_steal_pointer(&split);
    }

    return proc_cmdline;
}