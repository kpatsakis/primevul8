_kc_async_data_alloc(pid_t                   pid,
                     NMLogDomain             log_domain,
                     const char *            log_name,
                     NMUtilsKillChildAsyncCb callback,
                     void *                  user_data)
{
    KillChildAsyncData *data;
    size_t              log_name_len;

    /* append the name at the end of our KillChildAsyncData. */
    log_name_len = strlen(LOG_NAME_FMT) + 20 + strlen(log_name);
    data         = g_malloc(sizeof(KillChildAsyncData) - 1 + log_name_len);
    g_snprintf(data->log_name, log_name_len, LOG_NAME_FMT, LOG_NAME_ARGS);

    data->pid        = pid;
    data->user_data  = user_data;
    data->callback   = callback;
    data->log_domain = log_domain;

    return data;
}