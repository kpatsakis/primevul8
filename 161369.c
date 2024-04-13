handle_get_seat_id (GdmDBusDisplay        *skeleton,
                    GDBusMethodInvocation *invocation,
                    GdmDisplay            *self)
{
        char *seat_id;

        seat_id = NULL;
        gdm_display_get_seat_id (self, &seat_id, NULL);

        if (seat_id == NULL) {
                seat_id = g_strdup ("");
        }
        gdm_dbus_display_complete_get_seat_id (skeleton, invocation, seat_id);

        g_free (seat_id);
        return TRUE;
}