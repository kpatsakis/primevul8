add_watchdog_timer (GSWindow *window,
                    glong     timeout)
{
        window->priv->watchdog_timer_id = g_timeout_add_seconds (timeout,
                                                                 (GSourceFunc)watchdog_timer,
                                                                 window);
}