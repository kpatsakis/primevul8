spawn_on_window (GSWindow *window,
                 char     *command,
                 int      *pid,
                 GIOFunc   watch_func,
                 gpointer  user_data,
                 gint     *watch_id)
{
        int         argc;
        char      **argv;
        char      **envp;
        GError     *error;
        gboolean    result;
        GIOChannel *channel;
        int         standard_output;
        int         standard_error;
        int         child_pid;
        int         id;

        error = NULL;
        if (! g_shell_parse_argv (command, &argc, &argv, &error)) {
                gs_debug ("Could not parse command: %s", error->message);
                g_error_free (error);
                return FALSE;
        }

        envp = spawn_make_environment_for_screen (gtk_window_get_screen (GTK_WINDOW (window)), NULL);

        error = NULL;
        result = g_spawn_async_with_pipes (NULL,
                                           argv,
                                           envp,
                                           G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_SEARCH_PATH,
                                           NULL,
                                           NULL,
                                           &child_pid,
                                           NULL,
                                           &standard_output,
                                           &standard_error,
                                           &error);

        if (! result) {
                gs_debug ("Could not start command '%s': %s", command, error->message);
                g_error_free (error);
                g_strfreev (argv);
                return FALSE;
        }

        if (pid != NULL) {
                *pid = child_pid;
        } else {
                g_spawn_close_pid (child_pid);
        }

        /* output channel */
        channel = g_io_channel_unix_new (standard_output);
        g_io_channel_set_close_on_unref (channel, TRUE);
        g_io_channel_set_flags (channel,
                                g_io_channel_get_flags (channel) | G_IO_FLAG_NONBLOCK,
                                NULL);
        id = g_io_add_watch (channel,
                             G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_NVAL,
                             watch_func,
                             user_data);
        if (watch_id != NULL) {
                *watch_id = id;
        }
        g_io_channel_unref (channel);

        /* error channel */
        channel = g_io_channel_unix_new (standard_error);
        g_io_channel_set_close_on_unref (channel, TRUE);
        g_io_channel_set_flags (channel,
                                g_io_channel_get_flags (channel) | G_IO_FLAG_NONBLOCK,
                                NULL);
        id = g_io_add_watch (channel,
                             G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_NVAL,
                             error_watch,
                             NULL);
        g_io_channel_unref (channel);

        g_strfreev (argv);
        g_strfreev (envp);

        return result;
}