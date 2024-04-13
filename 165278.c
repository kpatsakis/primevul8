keyboard_command_finish (GSWindow *window)
{
        g_return_if_fail (GS_IS_WINDOW (window));

        /* send a signal just in case */
        kill_keyboard_command (window);

        gs_debug ("Keyboard finished");

        if (window->priv->keyboard_pid > 0) {
                wait_on_child (window->priv->keyboard_pid);

                g_spawn_close_pid (window->priv->keyboard_pid);
                window->priv->keyboard_pid = 0;
        }
}