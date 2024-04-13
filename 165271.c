gs_window_set_keyboard_command (GSWindow   *window,
                                const char *command)
{
        g_return_if_fail (GS_IS_WINDOW (window));

        g_free (window->priv->keyboard_command);

        if (command != NULL) {
                window->priv->keyboard_command = g_strdup (command);
        } else {
                window->priv->keyboard_command = NULL;
        }
}