handle_get_id (GdmDBusDisplay        *skeleton,
               GDBusMethodInvocation *invocation,
               GdmDisplay            *self)
{
        char *id;

        gdm_display_get_id (self, &id, NULL);

        gdm_dbus_display_complete_get_id (skeleton, invocation, id);

        g_free (id);
        return TRUE;
}