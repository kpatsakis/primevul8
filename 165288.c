popup_dialog (GSWindow *window)
{
        gboolean  result;
        char     *tmp;
        GString  *command;

        gs_debug ("Popping up dialog");

        tmp = g_build_filename (LIBEXECDIR, "cinnamon-screensaver-dialog", NULL);
        command = g_string_new (tmp);
        g_free (tmp);

        if (is_logout_enabled (window)) {
                command = g_string_append (command, " --enable-logout");
                g_string_append_printf (command, " --logout-command='%s'", window->priv->logout_command);
        }

        if (is_user_switch_enabled (window)) {
                command = g_string_append (command, " --enable-switch");
        }

        if (gs_debug_enabled ()) {
                command = g_string_append (command, " --verbose");
        }

        set_invisible_cursor (gtk_widget_get_window (GTK_WIDGET (window)), FALSE);

        window->priv->dialog_quit_requested = FALSE;
        window->priv->dialog_shake_in_progress = FALSE;

        result = spawn_on_window (window,
                                  command->str,
                                  &window->priv->lock_pid,
                                  (GIOFunc)lock_command_watch,
                                  window,
                                  &window->priv->lock_watch_id);
        if (! result) {
                gs_debug ("Could not start command: %s", command->str);
        }

        g_string_free (command, TRUE);
}