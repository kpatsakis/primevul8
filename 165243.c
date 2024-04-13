spawn_make_environment_for_screen (GdkScreen  *screen,
                                   gchar     **envp)
{
        gchar **retval = NULL;
        gchar  *display_name;
        gint    display_index = -1;
        gint    i, env_len;

        g_return_val_if_fail (GDK_IS_SCREEN (screen), NULL);

        if (envp == NULL)
                envp = environ;

        for (env_len = 0; envp[env_len]; env_len++)
                if (strncmp (envp[env_len], "DISPLAY", strlen ("DISPLAY")) == 0)
                        display_index = env_len;

        retval = g_new (char *, env_len + 1);
        retval[env_len] = NULL;

        display_name = gdk_screen_make_display_name (screen);

        for (i = 0; i < env_len; i++)
                if (i == display_index)
                        retval[i] = g_strconcat ("DISPLAY=", display_name, NULL);
                else
                        retval[i] = g_strdup (envp[i]);

        g_assert (i == env_len);

        g_free (display_name);

        return retval;
}