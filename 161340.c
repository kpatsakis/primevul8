_gdm_display_set_x11_display_name (GdmDisplay     *self,
                                   const char     *x11_display)
{
        GdmDisplayPrivate *priv;

        priv = gdm_display_get_instance_private (self);
        g_free (priv->x11_display_name);
        priv->x11_display_name = g_strdup (x11_display);
}