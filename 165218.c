lock_command_watch (GIOChannel   *source,
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
                        gs_debug ("command output: %s", line);

                        if (strstr (line, "WINDOW ID=") != NULL) {
                                guint32 id;
                                char    c;
                                if (1 == sscanf (line, " WINDOW ID= %" G_GUINT32_FORMAT " %c", &id, &c)) {
                                        create_lock_socket (window, id);
                                }
                        } else if (strstr (line, "NOTICE=") != NULL) {
                                if (strstr (line, "NOTICE=AUTH FAILED") != NULL) {
                                        shake_dialog (window);
                                }
                        } else if (strstr (line, "RESPONSE=") != NULL) {
                                if (strstr (line, "RESPONSE=OK") != NULL) {
                                        gs_debug ("Got OK response");
                                        window->priv->dialog_response = DIALOG_RESPONSE_OK;
                                } else {
                                        gs_debug ("Got CANCEL response");
                                        window->priv->dialog_response = DIALOG_RESPONSE_CANCEL;
                                }
                                finished = TRUE;
                        } else if (strstr (line, "REQUEST QUIT") != NULL) {
                                gs_debug ("Got request for quit");
                                window->priv->dialog_quit_requested = TRUE;
                                maybe_kill_dialog (window);
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
                popdown_dialog (window);

                if (window->priv->dialog_response == DIALOG_RESPONSE_OK) {
                        add_emit_deactivated_idle (window);
                }

                window->priv->lock_watch_id = 0;

                return FALSE;
        }

        return TRUE;
}