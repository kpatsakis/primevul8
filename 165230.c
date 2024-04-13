keyboard_command_watch (GIOChannel   *source,
                        GIOCondition  condition,
                        GSWindow     *window)
{
        gboolean finished = FALSE;

        g_return_val_if_fail (GS_IS_WINDOW (window), FALSE);

        if (condition & G_IO_IN) {
                GIOStatus status;
                GError   *error = NULL;
                char     *line;

                line = NULL;
                status = g_io_channel_read_line (source, &line, NULL, NULL, &error);

                switch (status) {
                case G_IO_STATUS_NORMAL:
                        {
                                guint32 id;
                                char    c;
                                gs_debug ("keyboard command output: %s", line);
                                if (1 == sscanf (line, " %" G_GUINT32_FORMAT " %c", &id, &c)) {
                                        create_keyboard_socket (window, id);
                                }
                        }
                        break;
                case G_IO_STATUS_EOF:
                        finished = TRUE;
                        break;
                case G_IO_STATUS_ERROR:
                        finished = TRUE;
                        gs_debug ("Error reading from child: %s\n", error->message);
                        g_error_free (error);
                        return FALSE;
                case G_IO_STATUS_AGAIN:
                default:
                        break;
                }

                g_free (line);
        } else if (condition & G_IO_HUP) {
                finished = TRUE;
        }

        if (finished) {
                window->priv->keyboard_watch_id = 0;
                keyboard_command_finish (window);
                return FALSE;
        }

        return TRUE;
}