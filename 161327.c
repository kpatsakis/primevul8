gdm_display_get_remote_hostname (GdmDisplay *self,
                                 char      **hostname,
                                 GError    **error)
{
        GdmDisplayPrivate *priv;

        g_return_val_if_fail (GDM_IS_DISPLAY (self), FALSE);

        priv = gdm_display_get_instance_private (self);
        if (hostname != NULL) {
                *hostname = g_strdup (priv->remote_hostname);
        }

        return TRUE;
}