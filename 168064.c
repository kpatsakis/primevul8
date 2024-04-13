_kc_invoke_callback(pid_t                   pid,
                    NMLogDomain             log_domain,
                    const char *            log_name,
                    NMUtilsKillChildAsyncCb callback,
                    void *                  user_data,
                    gboolean                success,
                    int                     child_status)
{
    KillChildAsyncData *data;

    if (!callback)
        return;

    data                    = _kc_async_data_alloc(pid, log_domain, log_name, callback, user_data);
    data->sync.success      = success;
    data->sync.child_status = child_status;

    g_idle_add(_kc_invoke_callback_idle, data);
}