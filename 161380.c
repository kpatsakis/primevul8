kernel_cmdline_initial_setup_force_state (gboolean *force_state)
{
        GError *error = NULL;
        gchar *contents = NULL;
        gchar *setup_argument = NULL;

        g_return_val_if_fail (force_state != NULL, FALSE);

        if (!g_file_get_contents ("/proc/cmdline", &contents, NULL, &error)) {
                g_debug ("GdmDisplay: Could not check kernel parameters, not forcing initial setup: %s",
                          error->message);
                g_clear_error (&error);
                return FALSE;
        }

        g_debug ("GdmDisplay: Checking kernel command buffer %s", contents);

        if (!kernel_cmdline_initial_setup_argument (contents, &setup_argument, &error)) {
                g_debug ("GdmDisplay: Failed to read kernel commandline: %s", error->message);
                g_clear_pointer (&contents, g_free);
                return FALSE;
        }

        g_clear_pointer (&contents, g_free);

        /* Poor-man's check for truthy or falsey values */
        *force_state = setup_argument[0] == '1';

        g_free (setup_argument);
        return TRUE;
}