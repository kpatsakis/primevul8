Opal::Call::toggle_hold ()
{
  bool on_hold = false;
  PSafePtr<OpalConnection> connection = get_remote_connection ();
  if (connection != NULL) {

    on_hold = connection->IsOnHold (false);
    if (!on_hold)
      connection->Hold (false, true);
    else
      connection->Hold (false, false);
  }
}