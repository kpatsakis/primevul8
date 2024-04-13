remove_watchdog_timer (GSWindow *window)
{
        if (window->priv->watchdog_timer_id != 0) {
                g_source_remove (window->priv->watchdog_timer_id);
                window->priv->watchdog_timer_id = 0;
        }
}