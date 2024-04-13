_kc_waited_to_string(char *buf, gint64 wait_start_us)
#define _kc_waited_to_string(buf, wait_start_us)                                        \
    (G_STATIC_ASSERT_EXPR(sizeof(buf) == KC_WAITED_TO_STRING && sizeof((buf)[0]) == 1), \
     _kc_waited_to_string(buf, wait_start_us))
{
    g_snprintf(buf,
               KC_WAITED_TO_STRING,
               " (%ld usec elapsed)",
               (long) (nm_utils_get_monotonic_timestamp_usec() - wait_start_us));
    return buf;
}