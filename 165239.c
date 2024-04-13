error_watch (GIOChannel   *source,
             GIOCondition  condition,
             gpointer      data)
{
        gboolean finished = FALSE;

        if (condition & G_IO_IN) {
                GIOStatus status;
                GError   *error = NULL;
                char     *line;

                line = NULL;
                status = g_io_channel_read_line (source, &line, NULL, NULL, &error);

                switch (status) {
                case G_IO_STATUS_NORMAL:
                        gs_debug ("command error output: %s", line);
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
                return FALSE;
        }

        return TRUE;
}