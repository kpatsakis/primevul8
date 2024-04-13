_sleep_duration_convert_ms_to_us(guint32 sleep_duration_msec)
{
    if (sleep_duration_msec > 0) {
        guint64 x = (gint64) sleep_duration_msec * (guint64) 1000L;

        return x < G_MAXULONG ? (gulong) x : G_MAXULONG;
    }
    return G_USEC_PER_SEC / 20;
}