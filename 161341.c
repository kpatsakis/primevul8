gdm_display_get_id (GdmDisplay         *self,
                    char              **id,
                    GError            **error)
{
        GdmDisplayPrivate *priv;

        g_return_val_if_fail (GDM_IS_DISPLAY (self), FALSE);

        priv = gdm_display_get_instance_private (self);
        if (id != NULL) {
                *id = g_strdup (priv->id);
        }

        return TRUE;
}