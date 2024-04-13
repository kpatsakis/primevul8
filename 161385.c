gdm_display_add_user_authorization (GdmDisplay *self,
                                    const char *username,
                                    char      **filename,
                                    GError    **error)
{
        GdmDisplayPrivate    *priv;
        GdmDisplayAccessFile *access_file;
        GError               *access_file_error;
        gboolean              res;

        int                       i;
        XHostAddress              host_entries[3];
        xcb_void_cookie_t         cookies[3];

        g_return_val_if_fail (GDM_IS_DISPLAY (self), FALSE);

        priv = gdm_display_get_instance_private (self);

        g_debug ("GdmDisplay: Adding authorization for user:%s on display %s", username, priv->x11_display_name);

        if (priv->user_access_file != NULL) {
                g_set_error (error,
                             G_DBUS_ERROR,
                             G_DBUS_ERROR_ACCESS_DENIED,
                             "user access already assigned");
                return FALSE;
        }

        g_debug ("GdmDisplay: Adding user authorization for %s", username);

        access_file_error = NULL;
        access_file = _create_access_file_for_user (self,
                                                    username,
                                                    &access_file_error);

        if (access_file == NULL) {
                g_propagate_error (error, access_file_error);
                return FALSE;
        }

        res = gdm_display_access_file_add_display_with_cookie (access_file,
                                                               self,
                                                               priv->x11_cookie,
                                                               priv->x11_cookie_size,
                                                               &access_file_error);
        if (! res) {
                g_debug ("GdmDisplay: Unable to add user authorization for %s: %s",
                         username,
                         access_file_error->message);
                g_propagate_error (error, access_file_error);
                gdm_display_access_file_close (access_file);
                g_object_unref (access_file);
                return FALSE;
        }

        *filename = gdm_display_access_file_get_path (access_file);
        priv->user_access_file = access_file;

        g_debug ("GdmDisplay: Added user authorization for %s: %s", username, *filename);
        /* Remove access for the programs run by greeter now that the
         * user session is starting.
         */
        setup_xhost_auth (host_entries);

        for (i = 0; i < G_N_ELEMENTS (host_entries); i++) {
                cookies[i] = xcb_change_hosts_checked (priv->xcb_connection,
                                                       XCB_HOST_MODE_DELETE,
                                                       host_entries[i].family,
                                                       host_entries[i].length,
                                                       (uint8_t *) host_entries[i].address);
        }

        for (i = 0; i < G_N_ELEMENTS (cookies); i++) {
                xcb_generic_error_t *xcb_error;

                xcb_error = xcb_request_check (priv->xcb_connection, cookies[i]);

                if (xcb_error != NULL) {
                        g_warning ("Failed to remove greeter program access to the display. Trying to proceed.");
                        free (xcb_error);
                }
        }

        return TRUE;
}