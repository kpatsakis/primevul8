goto_state (DBusAuth *auth,
            const DBusAuthStateData *state)
{
  _dbus_verbose ("%s: going from state %s to state %s\n",
                 DBUS_AUTH_NAME (auth),
                 auth->state->name,
                 state->name);

  auth->state = state;
}