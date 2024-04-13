_gdm_display_set_remote_hostname (GdmDisplay     *self,
                                  const char     *hostname)
{
        GdmDisplayPrivate *priv;

        priv = gdm_display_get_instance_private (self);
        g_free (priv->remote_hostname);
        priv->remote_hostname = g_strdup (hostname);
}