kill_keyboard_command (GSWindow *window)
{
        if (window->priv->keyboard_pid > 0) {
                signal_pid (window->priv->keyboard_pid, SIGTERM);
        }
}