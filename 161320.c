gdm_display_get_x11_cookie (GdmDisplay  *self,
                            const char **x11_cookie,
                            gsize       *x11_cookie_size,
                            GError     **error)
{
        GdmDisplayPrivate *priv;

        g_return_val_if_fail (GDM_IS_DISPLAY (self), FALSE);

        priv = gdm_display_get_instance_private (self);

        if (x11_cookie != NULL) {
                *x11_cookie = priv->x11_cookie;
        }

        if (x11_cookie_size != NULL) {
                *x11_cookie_size = priv->x11_cookie_size;
        }

        return TRUE;
}