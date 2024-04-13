_kc_cb_timeout_grace_period(void *user_data)
{
    KillChildAsyncData *data = user_data;
    int                 ret, errsv;

    data->async.source_timeout_kill_id = 0;

    if ((ret = kill(data->pid, SIGKILL)) != 0) {
        errsv = errno;
        /* ESRCH means, process does not exist or is already a zombie. */
        if (errsv != ESRCH) {
            nm_log_err(LOGD_CORE | data->log_domain,
                       "%s: kill(SIGKILL) returned unexpected return value %d: (%s, %d)",
                       data->log_name,
                       ret,
                       nm_strerror_native(errsv),
                       errsv);
        }
    } else {
        nm_log_dbg(data->log_domain,
                   "%s: process not terminated after %ld usec. Sending SIGKILL signal",
                   data->log_name,
                   (long) (nm_utils_get_monotonic_timestamp_usec() - data->async.wait_start_us));
    }

    return G_SOURCE_REMOVE;
}