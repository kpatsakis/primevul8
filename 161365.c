gdm_display_is_local (GdmDisplay *self,
                      gboolean   *local,
                      GError    **error)
{
        GdmDisplayPrivate *priv;

        g_return_val_if_fail (GDM_IS_DISPLAY (self), FALSE);

        priv = gdm_display_get_instance_private (self);
        if (local != NULL) {
                *local = priv->is_local;
        }

        return TRUE;
}