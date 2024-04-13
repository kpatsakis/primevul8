gdm_display_unmanage (GdmDisplay *self)
{
        GdmDisplayPrivate *priv;

        g_return_val_if_fail (GDM_IS_DISPLAY (self), FALSE);

        priv = gdm_display_get_instance_private (self);

        g_debug ("GdmDisplay: unmanage display");

        gdm_display_disconnect (self);

        if (priv->user_access_file != NULL) {
                gdm_display_access_file_close (priv->user_access_file);
                g_object_unref (priv->user_access_file);
                priv->user_access_file = NULL;
        }

        if (priv->access_file != NULL) {
                gdm_display_access_file_close (priv->access_file);
                g_object_unref (priv->access_file);
                priv->access_file = NULL;
        }

        if (!priv->session_registered) {
                g_warning ("GdmDisplay: Session never registered, failing");
                _gdm_display_set_status (self, GDM_DISPLAY_FAILED);
        } else {
                _gdm_display_set_status (self, GDM_DISPLAY_UNMANAGED);
        }

        return TRUE;
}