_gdm_display_set_x11_cookie (GdmDisplay     *self,
                             const char     *x11_cookie)
{
        GdmDisplayPrivate *priv;

        priv = gdm_display_get_instance_private (self);
        g_free (priv->x11_cookie);
        priv->x11_cookie = g_strdup (x11_cookie);
}