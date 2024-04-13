_kc_invoke_callback_idle(gpointer user_data)
{
    KillChildAsyncData *data = user_data;

    if (data->sync.success) {
        char buf_exit[KC_EXIT_TO_STRING_BUF_SIZE];

        nm_log_dbg(data->log_domain,
                   "%s: invoke callback: terminated %s",
                   data->log_name,
                   _kc_exit_to_string(buf_exit, data->sync.child_status));
    } else
        nm_log_dbg(data->log_domain, "%s: invoke callback: killing child failed", data->log_name);

    data->callback(data->pid, data->sync.success, data->sync.child_status, data->user_data);
    g_free(data);

    return G_SOURCE_REMOVE;
}