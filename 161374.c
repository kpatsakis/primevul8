gdm_display_remove_user_authorization (GdmDisplay *self,
                                       const char *username,
                                       GError    **error)
{
        GdmDisplayPrivate *priv;

        g_return_val_if_fail (GDM_IS_DISPLAY (self), FALSE);

        priv = gdm_display_get_instance_private (self);

        g_debug ("GdmDisplay: Removing authorization for user:%s on display %s", username, priv->x11_display_name);

        gdm_display_access_file_close (priv->user_access_file);

        return TRUE;
}