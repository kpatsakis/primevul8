_kc_cb_watch_child(GPid pid, int status, gpointer user_data)
{
    KillChildAsyncData *data = user_data;
    char                buf_exit[KC_EXIT_TO_STRING_BUF_SIZE], buf_wait[KC_WAITED_TO_STRING];

    if (data->async.source_timeout_kill_id)
        g_source_remove(data->async.source_timeout_kill_id);

    nm_log_dbg(data->log_domain,
               "%s: terminated %s%s",
               data->log_name,
               _kc_exit_to_string(buf_exit, status),
               _kc_waited_to_string(buf_wait, data->async.wait_start_us));

    if (data->callback)
        data->callback(pid, TRUE, status, data->user_data);

    g_free(data);
}