Opal::Call::OnAlerting (OpalConnection & connection)
{
  if (!PIsDescendant(&connection, OpalPCSSConnection))
    Ekiga::Runtime::run_in_main (boost::bind (&Opal::Call::emit_ringing_in_main, this));

  return OpalCall::OnAlerting (connection);
}