Opal::Call::answer ()
{
  if (!is_outgoing () && !IsEstablished ()) {
    PSafePtr<OpalPCSSConnection> connection = GetConnectionAs<OpalPCSSConnection>();
    if (connection != NULL) {
      connection->AcceptIncoming();
    }
  }
}