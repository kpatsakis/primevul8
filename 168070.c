nm_utils_kill_child_sync(pid_t       pid,
                         int         sig,
                         NMLogDomain log_domain,
                         const char *log_name,
                         int *       child_status,
                         guint32     wait_before_kill_msec,
                         guint32     sleep_duration_msec)
{
    int      status = 0, errsv = 0;
    pid_t    ret;
    gboolean success     = FALSE;
    gboolean was_waiting = FALSE, send_kill = FALSE;
    char     buf_exit[KC_EXIT_TO_STRING_BUF_SIZE];
    char     buf_wait[KC_WAITED_TO_STRING];
    gint64   wait_start_us;

    g_return_val_if_fail(pid > 0, FALSE);
    g_return_val_if_fail(log_name != NULL, FALSE);

    /* check if the child process already terminated... */
    ret = waitpid(pid, &status, WNOHANG);
    if (ret > 0) {
        nm_log_dbg(log_domain,
                   LOG_NAME_FMT ": process %ld already terminated %s",
                   LOG_NAME_ARGS,
                   (long) ret,
                   _kc_exit_to_string(buf_exit, status));
        success = TRUE;
        goto out;
    } else if (ret != 0) {
        errsv = errno;
        /* ECHILD means, the process is not a child/does not exist or it has SIGCHILD blocked. */
        if (errsv != ECHILD) {
            nm_log_err(LOGD_CORE | log_domain,
                       LOG_NAME_FMT ": unexpected error while waitpid: %s (%d)",
                       LOG_NAME_ARGS,
                       nm_strerror_native(errsv),
                       errsv);
            goto out;
        }
    }

    /* send first signal @sig */
    if (kill(pid, sig) != 0) {
        errsv = errno;
        /* ESRCH means, process does not exist or is already a zombie. */
        if (errsv != ESRCH) {
            nm_log_err(LOGD_CORE | log_domain,
                       LOG_NAME_FMT ": failed to send %s: %s (%d)",
                       LOG_NAME_ARGS,
                       _kc_signal_to_string(sig),
                       nm_strerror_native(errsv),
                       errsv);
        } else {
            /* let's try again with waitpid, probably there was a race... */
            ret = waitpid(pid, &status, 0);
            if (ret > 0) {
                nm_log_dbg(log_domain,
                           LOG_NAME_FMT ": process %ld already terminated %s",
                           LOG_NAME_ARGS,
                           (long) ret,
                           _kc_exit_to_string(buf_exit, status));
                success = TRUE;
            } else {
                errsv = errno;
                nm_log_err(LOGD_CORE | log_domain,
                           LOG_NAME_FMT ": failed due to unexpected return value %ld by waitpid "
                                        "(%s, %d) after sending %s",
                           LOG_NAME_ARGS,
                           (long) ret,
                           nm_strerror_native(errsv),
                           errsv,
                           _kc_signal_to_string(sig));
            }
        }
        goto out;
    }

    wait_start_us = nm_utils_get_monotonic_timestamp_usec();

    /* wait for the process to terminated... */
    if (sig != SIGKILL) {
        gint64 wait_until, now;
        gulong sleep_time, sleep_duration_usec;
        int    loop_count = 0;

        sleep_duration_usec = _sleep_duration_convert_ms_to_us(sleep_duration_msec);
        wait_until          = wait_before_kill_msec <= 0
                                  ? 0
                                  : wait_start_us + (((gint64) wait_before_kill_msec) * 1000L);

        while (TRUE) {
            ret = waitpid(pid, &status, WNOHANG);
            if (ret > 0) {
                nm_log_dbg(log_domain,
                           LOG_NAME_FMT ": after sending %s, process %ld exited %s%s",
                           LOG_NAME_ARGS,
                           _kc_signal_to_string(sig),
                           (long) ret,
                           _kc_exit_to_string(buf_exit, status),
                           was_waiting ? _kc_waited_to_string(buf_wait, wait_start_us) : "");
                success = TRUE;
                goto out;
            }
            if (ret == -1) {
                errsv = errno;
                /* ECHILD means, the process is not a child/does not exist or it has SIGCHILD blocked. */
                if (errsv != ECHILD) {
                    nm_log_err(LOGD_CORE | log_domain,
                               LOG_NAME_FMT ": after sending %s, waitpid failed with %s (%d)%s",
                               LOG_NAME_ARGS,
                               _kc_signal_to_string(sig),
                               nm_strerror_native(errsv),
                               errsv,
                               was_waiting ? _kc_waited_to_string(buf_wait, wait_start_us) : "");
                    goto out;
                }
            }

            if (!wait_until)
                break;

            now = nm_utils_get_monotonic_timestamp_usec();
            if (now >= wait_until)
                break;

            if (!was_waiting) {
                nm_log_dbg(log_domain,
                           LOG_NAME_FMT ": waiting up to %ld milliseconds for process to terminate "
                                        "normally after sending %s...",
                           LOG_NAME_ARGS,
                           (long) MAX(wait_before_kill_msec, 0),
                           _kc_signal_to_string(sig));
                was_waiting = TRUE;
            }

            sleep_time = MIN(wait_until - now, sleep_duration_usec);
            if (loop_count < 20) {
                /* At the beginning we expect the process to die fast.
                 * Limit the sleep time, the limit doubles with every iteration. */
                sleep_time = MIN(sleep_time, (((guint64) 1) << loop_count) * G_USEC_PER_SEC / 2000);
                loop_count++;
            }
            g_usleep(sleep_time);
        }

        /* send SIGKILL, if called with @wait_before_kill_msec > 0 */
        if (wait_until) {
            nm_log_dbg(log_domain, LOG_NAME_FMT ": sending SIGKILL...", LOG_NAME_ARGS);

            send_kill = TRUE;
            if (kill(pid, SIGKILL) != 0) {
                errsv = errno;
                /* ESRCH means, process does not exist or is already a zombie. */
                if (errsv != ESRCH) {
                    nm_log_err(LOGD_CORE | log_domain,
                               LOG_NAME_FMT ": failed to send SIGKILL (after sending %s), %s (%d)",
                               LOG_NAME_ARGS,
                               _kc_signal_to_string(sig),
                               nm_strerror_native(errsv),
                               errsv);
                    goto out;
                }
            }
        }
    }

    if (!was_waiting) {
        nm_log_dbg(log_domain,
                   LOG_NAME_FMT ": waiting for process to terminate after sending %s%s...",
                   LOG_NAME_ARGS,
                   _kc_signal_to_string(sig),
                   send_kill ? " and SIGKILL" : "");
    }

    /* block until the child terminates. */
    while ((ret = waitpid(pid, &status, 0)) <= 0) {
        errsv = errno;

        if (errsv != EINTR) {
            nm_log_err(LOGD_CORE | log_domain,
                       LOG_NAME_FMT ": after sending %s%s, waitpid failed with %s (%d)%s",
                       LOG_NAME_ARGS,
                       _kc_signal_to_string(sig),
                       send_kill ? " and SIGKILL" : "",
                       nm_strerror_native(errsv),
                       errsv,
                       _kc_waited_to_string(buf_wait, wait_start_us));
            goto out;
        }
    }

    nm_log_dbg(log_domain,
               LOG_NAME_FMT ": after sending %s%s, process %ld exited %s%s",
               LOG_NAME_ARGS,
               _kc_signal_to_string(sig),
               send_kill ? " and SIGKILL" : "",
               (long) ret,
               _kc_exit_to_string(buf_exit, status),
               _kc_waited_to_string(buf_wait, wait_start_us));
    success = TRUE;
out:
    if (child_status)
        *child_status = success ? status : -1;
    errno = success ? 0 : errsv;
    return success;
}