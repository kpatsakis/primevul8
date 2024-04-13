maybe_kill_dialog (GSWindow *window)
{
        if (!window->priv->dialog_shake_in_progress
            && window->priv->dialog_quit_requested
            && window->priv->lock_pid > 0) {
                kill (window->priv->lock_pid, SIGTERM);
        }
}