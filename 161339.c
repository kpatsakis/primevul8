gdm_display_is_initial (GdmDisplay  *self,
                        gboolean    *is_initial,
                        GError     **error)
{
        GdmDisplayPrivate *priv;

        g_return_val_if_fail (GDM_IS_DISPLAY (self), FALSE);

        priv = gdm_display_get_instance_private (self);
        if (is_initial != NULL) {
                *is_initial = priv->is_initial;
        }

        return TRUE;
}