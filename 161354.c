gdm_display_get_x11_authority_file (GdmDisplay *self,
                                    char      **filename,
                                    GError    **error)
{
        GdmDisplayPrivate *priv;

        g_return_val_if_fail (GDM_IS_DISPLAY (self), FALSE);
        g_return_val_if_fail (filename != NULL, FALSE);

        priv = gdm_display_get_instance_private (self);
        if (priv->access_file != NULL) {
                *filename = gdm_display_access_file_get_path (priv->access_file);
        } else {
                *filename = NULL;
        }

        return TRUE;
}