is_logout_enabled (GSWindow *window)
{
        double elapsed;

        if (! window->priv->logout_enabled) {
                return FALSE;
        }

        if (! window->priv->logout_command) {
                return FALSE;
        }

        elapsed = g_timer_elapsed (window->priv->timer, NULL);

        if (window->priv->logout_timeout < (elapsed * 1000)) {
                return TRUE;
        }

        return FALSE;
}