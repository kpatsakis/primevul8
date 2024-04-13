Opal::Call::send_dtmf (const char dtmf)
{
  PSafePtr<OpalConnection> connection = get_remote_connection ();
  if (connection != NULL) {
    connection->SendUserInputTone (dtmf, 180);
  }
}