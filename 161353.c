can_create_environment (const char *session_id)
{
        char *path;
        gboolean session_exists;

        path = g_strdup_printf (GNOME_SESSION_SESSIONS_PATH "/%s.session", session_id);
        session_exists = g_file_test (path, G_FILE_TEST_EXISTS);

        g_free (path);

        return session_exists;
}