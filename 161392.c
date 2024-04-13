_create_access_file_for_user (GdmDisplay  *self,
                              const char  *username,
                              GError     **error)
{
        GdmDisplayAccessFile *access_file;
        GError *file_error;

        access_file = gdm_display_access_file_new (username);

        file_error = NULL;
        if (!gdm_display_access_file_open (access_file, &file_error)) {
                g_propagate_error (error, file_error);
                return NULL;
        }

        return access_file;
}