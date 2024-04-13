gdm_display_get_x11_display_name (GdmDisplay   *self,
                                  char        **x11_display,
                                  GError      **error)
{
        GdmDisplayPrivate *priv;

        g_return_val_if_fail (GDM_IS_DISPLAY (self), FALSE);

        priv = gdm_display_get_instance_private (self);
        if (x11_display != NULL) {
                *x11_display = g_strdup (priv->x11_display_name);
        }

        return TRUE;
}