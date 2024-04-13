on_clock_changed (GnomeWallClock *clock,
                  GParamSpec     *pspec,
                  gpointer        user_data)
{
        update_clock (GS_WINDOW (user_data));
}