gdm_display_create_authority (GdmDisplay *self)
{
        GdmDisplayPrivate    *priv;
        GdmDisplayAccessFile *access_file;
        GError               *error;
        gboolean              res;

        g_return_val_if_fail (GDM_IS_DISPLAY (self), FALSE);

        priv = gdm_display_get_instance_private (self);
        g_return_val_if_fail (priv->access_file == NULL, FALSE);

        error = NULL;
        access_file = _create_access_file_for_user (self, GDM_USERNAME, &error);

        if (access_file == NULL) {
                g_critical ("could not create display access file: %s", error->message);
                g_error_free (error);
                return FALSE;
        }

        g_free (priv->x11_cookie);
        priv->x11_cookie = NULL;
        res = gdm_display_access_file_add_display (access_file,
                                                   self,
                                                   &priv->x11_cookie,
                                                   &priv->x11_cookie_size,
                                                   &error);

        if (! res) {

                g_critical ("could not add display to access file: %s", error->message);
                g_error_free (error);
                gdm_display_access_file_close (access_file);
                g_object_unref (access_file);
                return FALSE;
        }

        priv->access_file = access_file;

        return TRUE;
}