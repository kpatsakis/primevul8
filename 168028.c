nm_utils_proc_cmdline(void)
{
    static const char *volatile proc_cmdline_cached = NULL;
    const char *proc_cmdline;

again:
    proc_cmdline = g_atomic_pointer_get(&proc_cmdline_cached);
    if (G_UNLIKELY(!proc_cmdline)) {
        gs_free char *str = NULL;

        g_file_get_contents("/proc/cmdline", &str, NULL, NULL);
        str = nm_str_realloc(str);

        proc_cmdline = str ?: "";
        if (!g_atomic_pointer_compare_and_exchange(&proc_cmdline_cached, NULL, proc_cmdline))
            goto again;

        g_steal_pointer(&str);
    }

    return proc_cmdline;
}