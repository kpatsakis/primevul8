gs_window_dialog_finish (GSWindow *window)
{
        g_return_if_fail (GS_IS_WINDOW (window));

        gs_debug ("Dialog finished");

        /* make sure we finish the keyboard thing too */
        keyboard_command_finish (window);

        /* send a signal just in case */
        kill_dialog_command (window);

        if (window->priv->lock_pid > 0) {
                wait_on_child (window->priv->lock_pid);

                g_spawn_close_pid (window->priv->lock_pid);
                window->priv->lock_pid = 0;
        }

        /* remove events for the case were we failed to show socket */
        remove_key_events (window);
}