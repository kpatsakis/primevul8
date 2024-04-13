kill_dialog_command (GSWindow *window)
{
        /* If a dialog is up we need to signal it
           and wait on it */
        if (window->priv->lock_pid > 0) {
                signal_pid (window->priv->lock_pid, SIGTERM);
        }
}