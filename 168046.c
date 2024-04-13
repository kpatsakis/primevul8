nm_utils_kill_process_sync(pid_t       pid,
                           guint64     start_time,
                           int         sig,
                           NMLogDomain log_domain,
                           const char *log_name,
                           guint32     wait_before_kill_msec,
                           guint32     sleep_duration_msec,
                           guint32     max_wait_msec)
{
    int      errsv;
    guint64  start_time0;
    gint64   wait_until_sigkill, now, wait_start_us, max_wait_until;
    gulong   sleep_time, sleep_duration_usec;
    int      loop_count  = 0;
    gboolean was_waiting = FALSE;
    char     buf_wait[KC_WAITED_TO_STRING];
    char     p_state;

    g_return_if_fail(pid > 0);
    g_return_if_fail(log_name != NULL);

    start_time0 = nm_utils_get_start_time_for_pid(pid, &p_state, NULL);
    if (start_time0 == 0) {
        nm_log_dbg(log_domain,
                   LOG_NAME_PROCESS_FMT ": cannot kill process %ld because it seems already gone",
                   LOG_NAME_ARGS,
                   (long int) pid);
        return;
    }
    if (start_time != 0 && start_time != start_time0) {
        nm_log_dbg(
            log_domain,
            LOG_NAME_PROCESS_FMT
            ": don't kill process %ld because the start_time is unexpectedly %lu instead of %ld",
            LOG_NAME_ARGS,
            (long int) pid,
            (unsigned long) start_time0,
            (unsigned long) start_time);
        return;
    }

    switch (p_state) {
    case 'Z':
    case 'x':
    case 'X':
        nm_log_dbg(log_domain,
                   LOG_NAME_PROCESS_FMT
                   ": cannot kill process %ld because it is already a zombie (%c)",
                   LOG_NAME_ARGS,
                   (long int) pid,
                   p_state);
        return;
    default:
        break;
    }

    if (kill(pid, sig) != 0) {
        errsv = errno;
        /* ESRCH means, process does not exist or is already a zombie. */
        if (errsv == ESRCH) {
            nm_log_dbg(log_domain,
                       LOG_NAME_PROCESS_FMT ": failed to send %s because process seems gone",
                       LOG_NAME_ARGS,
                       _kc_signal_to_string(sig));
        } else {
            nm_log_warn(LOGD_CORE | log_domain,
                        LOG_NAME_PROCESS_FMT ": failed to send %s: %s (%d)",
                        LOG_NAME_ARGS,
                        _kc_signal_to_string(sig),
                        nm_strerror_native(errsv),
                        errsv);
        }
        return;
    }

    /* wait for the process to terminate... */

    wait_start_us = nm_utils_get_monotonic_timestamp_usec();

    sleep_duration_usec = _sleep_duration_convert_ms_to_us(sleep_duration_msec);
    if (sig != SIGKILL && wait_before_kill_msec)
        wait_until_sigkill = wait_start_us + (((gint64) wait_before_kill_msec) * 1000L);
    else
        wait_until_sigkill = 0;
    if (max_wait_msec > 0) {
        max_wait_until = wait_start_us + (((gint64) max_wait_msec) * 1000L);
        if (wait_until_sigkill > 0 && wait_until_sigkill > max_wait_msec)
            wait_until_sigkill = 0;
    } else
        max_wait_until = 0;

    while (TRUE) {
        start_time = nm_utils_get_start_time_for_pid(pid, &p_state, NULL);

        if (start_time != start_time0) {
            nm_log_dbg(log_domain,
                       LOG_NAME_PROCESS_FMT ": process is gone after sending signal %s%s",
                       LOG_NAME_ARGS,
                       _kc_signal_to_string(sig),
                       was_waiting ? _kc_waited_to_string(buf_wait, wait_start_us) : "");
            return;
        }
        switch (p_state) {
        case 'Z':
        case 'x':
        case 'X':
            nm_log_dbg(log_domain,
                       LOG_NAME_PROCESS_FMT ": process is a zombie (%c) after sending signal %s%s",
                       LOG_NAME_ARGS,
                       p_state,
                       _kc_signal_to_string(sig),
                       was_waiting ? _kc_waited_to_string(buf_wait, wait_start_us) : "");
            return;
        default:
            break;
        }

        if (kill(pid, 0) != 0) {
            errsv = errno;
            /* ESRCH means, process does not exist or is already a zombie. */
            if (errsv == ESRCH) {
                nm_log_dbg(log_domain,
                           LOG_NAME_PROCESS_FMT
                           ": process is gone or a zombie after sending signal %s%s",
                           LOG_NAME_ARGS,
                           _kc_signal_to_string(sig),
                           was_waiting ? _kc_waited_to_string(buf_wait, wait_start_us) : "");
            } else {
                nm_log_warn(LOGD_CORE | log_domain,
                            LOG_NAME_PROCESS_FMT ": failed to kill(%ld, 0): %s (%d)%s",
                            LOG_NAME_ARGS,
                            (long int) pid,
                            nm_strerror_native(errsv),
                            errsv,
                            was_waiting ? _kc_waited_to_string(buf_wait, wait_start_us) : "");
            }
            return;
        }

        sleep_time = sleep_duration_usec;
        now        = nm_utils_get_monotonic_timestamp_usec();

        if (max_wait_until != 0 && now >= max_wait_until) {
            if (wait_until_sigkill != 0) {
                /* wait_before_kill_msec is not larger then max_wait_until but we did not yet send
                 * SIGKILL. Although we already reached our timeout, we don't want to skip sending
                 * the signal. Even if we don't wait for the process to disappear. */
                nm_log_dbg(log_domain, LOG_NAME_PROCESS_FMT ": sending SIGKILL", LOG_NAME_ARGS);
                kill(pid, SIGKILL);
            }
            nm_log_warn(log_domain,
                        LOG_NAME_PROCESS_FMT
                        ": timeout %u msec waiting for process to disappear (after sending %s)%s",
                        LOG_NAME_ARGS,
                        (unsigned) max_wait_until,
                        _kc_signal_to_string(sig),
                        was_waiting ? _kc_waited_to_string(buf_wait, wait_start_us) : "");
            return;
        }

        if (wait_until_sigkill != 0) {
            if (now >= wait_until_sigkill) {
                /* Still not dead. SIGKILL now... */
                nm_log_dbg(log_domain, LOG_NAME_PROCESS_FMT ": sending SIGKILL", LOG_NAME_ARGS);
                if (kill(pid, SIGKILL) != 0) {
                    errsv = errno;
                    /* ESRCH means, process does not exist or is already a zombie. */
                    if (errsv != ESRCH) {
                        nm_log_dbg(log_domain,
                                   LOG_NAME_PROCESS_FMT ": process is gone or a zombie%s",
                                   LOG_NAME_ARGS,
                                   _kc_waited_to_string(buf_wait, wait_start_us));
                    } else {
                        nm_log_warn(LOGD_CORE | log_domain,
                                    LOG_NAME_PROCESS_FMT
                                    ": failed to send SIGKILL (after sending %s), %s (%d)%s",
                                    LOG_NAME_ARGS,
                                    _kc_signal_to_string(sig),
                                    nm_strerror_native(errsv),
                                    errsv,
                                    _kc_waited_to_string(buf_wait, wait_start_us));
                    }
                    return;
                }
                sig                = SIGKILL;
                wait_until_sigkill = 0;
                loop_count =
                    0; /* reset the loop_count. Now we really expect the process to die quickly. */
            } else
                sleep_time = MIN(wait_until_sigkill - now, sleep_duration_usec);
        }

        if (!was_waiting) {
            if (wait_until_sigkill != 0) {
                nm_log_dbg(log_domain,
                           LOG_NAME_PROCESS_FMT
                           ": waiting up to %ld milliseconds for process to disappear before "
                           "sending KILL signal after sending %s...",
                           LOG_NAME_ARGS,
                           (long) wait_before_kill_msec,
                           _kc_signal_to_string(sig));
            } else if (max_wait_until != 0) {
                nm_log_dbg(
                    log_domain,
                    LOG_NAME_PROCESS_FMT
                    ": waiting up to %ld milliseconds for process to disappear after sending %s...",
                    LOG_NAME_ARGS,
                    (long) max_wait_msec,
                    _kc_signal_to_string(sig));
            } else {
                nm_log_dbg(log_domain,
                           LOG_NAME_PROCESS_FMT
                           ": waiting for process to disappear after sending %s...",
                           LOG_NAME_ARGS,
                           _kc_signal_to_string(sig));
            }
            was_waiting = TRUE;
        }

        if (loop_count < 20) {
            /* At the beginning we expect the process to die fast.
             * Limit the sleep time, the limit doubles with every iteration. */
            sleep_time = MIN(sleep_time, (((guint64) 1) << loop_count) * G_USEC_PER_SEC / 2000);
            loop_count++;
        }
        g_usleep(sleep_time);
    }
}