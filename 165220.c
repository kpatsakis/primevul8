embed_keyboard (GSWindow *window)
{
        gboolean res;

        if (! window->priv->keyboard_enabled
            || window->priv->keyboard_command == NULL)
                return;

        gs_debug ("Adding embedded keyboard widget");

        /* FIXME: verify command is safe */

        gs_debug ("Running command: %s", window->priv->keyboard_command);

        res = spawn_on_window (window,
                               window->priv->keyboard_command,
                               &window->priv->keyboard_pid,
                               (GIOFunc)keyboard_command_watch,
                               window,
                               &window->priv->keyboard_watch_id);
        if (! res) {
                gs_debug ("Could not start command: %s", window->priv->keyboard_command);
        }
}