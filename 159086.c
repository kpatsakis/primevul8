Opal::Call::transfer (std::string uri)
{
  PSafePtr<OpalConnection> connection = get_remote_connection ();
  if (connection != NULL)
    connection->TransferConnection (uri);
}