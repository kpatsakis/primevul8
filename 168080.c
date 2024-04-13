nm_utils_kill_child_async(pid_t                   pid,
                          int                     sig,
                          NMLogDomain             log_domain,
                          const char *            log_name,
                          guint32                 wait_before_kill_msec,
                          NMUtilsKillChildAsyncCb callback,
                          void *                  user_data)
{
    int                 status = 0, errsv;
    pid_t               ret;
    KillChildAsyncData *data;
    char                buf_exit[KC_EXIT_TO_STRING_BUF_SIZE];

    g_return_if_fail(pid > 0);
    g_return_if_fail(log_name != NULL);

    /* let's see if the child already terminated... */
    ret = waitpid(pid, &status, WNOHANG);
    if (ret > 0) {
        nm_log_dbg(log_domain,
                   LOG_NAME_FMT ": process %ld already terminated %s",
                   LOG_NAME_ARGS,
                   (long) ret,
                   _kc_exit_to_string(buf_exit, status));
        _kc_invoke_callback(pid, log_domain, log_name, callback, user_data, TRUE, status);
        return;
    } else if (ret != 0) {
        errsv = errno;
        /* ECHILD means, the process is not a child/does not exist or it has SIGCHILD blocked. */
        if (errsv != ECHILD) {
            nm_log_err(LOGD_CORE | log_domain,
                       LOG_NAME_FMT ": unexpected error while waitpid: %s (%d)",
                       LOG_NAME_ARGS,
                       nm_strerror_native(errsv),
                       errsv);
            _kc_invoke_callback(pid, log_domain, log_name, callback, user_data, FALSE, -1);
            return;
        }
    }

    /* send the first signal. */
    if (kill(pid, sig) != 0) {
        errsv = errno;
        /* ESRCH means, process does not exist or is already a zombie. */
        if (errsv != ESRCH) {
            nm_log_err(LOGD_CORE | log_domain,
                       LOG_NAME_FMT ": unexpected error sending %s: %s (%d)",
                       LOG_NAME_ARGS,
                       _kc_signal_to_string(sig),
                       nm_strerror_native(errsv),
                       errsv);
            _kc_invoke_callback(pid, log_domain, log_name, callback, user_data, FALSE, -1);
            return;
        }

        /* let's try again with waitpid, probably there was a race... */
        ret = waitpid(pid, &status, 0);
        if (ret > 0) {
            nm_log_dbg(log_domain,
                       LOG_NAME_FMT ": process %ld already terminated %s",
                       LOG_NAME_ARGS,
                       (long) ret,
                       _kc_exit_to_string(buf_exit, status));
            _kc_invoke_callback(pid, log_domain, log_name, callback, user_data, TRUE, status);
        } else {
            errsv = errno;
            nm_log_err(
                LOGD_CORE | log_domain,
                LOG_NAME_FMT
                ": failed due to unexpected return value %ld by waitpid (%s, %d) after sending %s",
                LOG_NAME_ARGS,
                (long) ret,
                nm_strerror_native(errsv),
                errsv,
                _kc_signal_to_string(sig));
            _kc_invoke_callback(pid, log_domain, log_name, callback, user_data, FALSE, -1);
        }
        return;
    }

    data = _kc_async_data_alloc(pid, log_domain, log_name, callback, user_data);
    data->async.wait_start_us = nm_utils_get_monotonic_timestamp_usec();

    if (sig != SIGKILL && wait_before_kill_msec > 0) {
        data->async.source_timeout_kill_id =
            g_timeout_add(wait_before_kill_msec, _kc_cb_timeout_grace_period, data);
        nm_log_dbg(log_domain,
                   "%s: wait for process to terminate after sending %s (send SIGKILL in %ld "
                   "milliseconds)...",
                   data->log_name,
                   _kc_signal_to_string(sig),
                   (long) wait_before_kill_msec);
    } else {
        data->async.source_timeout_kill_id = 0;
        nm_log_dbg(log_domain,
                   "%s: wait for process to terminate after sending %s...",
                   data->log_name,
                   _kc_signal_to_string(sig));
    }

    g_child_watch_add(pid, _kc_cb_watch_child, data);
}