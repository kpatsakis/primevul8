gs_window_set_logout_command (GSWindow   *window,
                              const char *command)
{
        g_return_if_fail (GS_IS_WINDOW (window));

        g_free (window->priv->logout_command);

        if (command) {
                window->priv->logout_command = g_strdup (command);
        } else {
                window->priv->logout_command = NULL;
        }
}