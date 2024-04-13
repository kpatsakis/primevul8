gdm_display_manage (GdmDisplay *self)
{
        GdmDisplayPrivate *priv;
        gboolean res;

        g_return_val_if_fail (GDM_IS_DISPLAY (self), FALSE);

        priv = gdm_display_get_instance_private (self);

        g_debug ("GdmDisplay: Managing display: %s", priv->id);

        /* If not explicitly prepared, do it now */
        if (priv->status == GDM_DISPLAY_UNMANAGED) {
                res = gdm_display_prepare (self);
                if (! res) {
                        return FALSE;
                }
        }

        if (g_strcmp0 (priv->session_class, "greeter") == 0) {
                if (GDM_DISPLAY_GET_CLASS (self)->manage != NULL) {
                        GDM_DISPLAY_GET_CLASS (self)->manage (self);
                }
        }

        return TRUE;
}