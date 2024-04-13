Opal::Call::OnNoAnswerTimeout (PTimer &,
                               INT)
{
  if (!is_outgoing ()) {

    if (!forward_uri.empty ()) {

      PSafePtr<OpalConnection> connection = get_remote_connection ();
      if (connection != NULL)
        connection->ForwardCall (forward_uri);
    }
    else
      Clear (OpalConnection::EndedByNoAnswer);
  }
}