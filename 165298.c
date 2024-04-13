gs_window_set_away_message (GSWindow   *window,
                            const char *message)
{
        g_return_if_fail (GS_IS_WINDOW (window));

        g_free (window->priv->away_message);

        if (message) {
                window->priv->away_message = g_strdup (message);
        } else {
                window->priv->away_message = NULL;
        }

        update_clock (window);
}