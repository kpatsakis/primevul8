Opal::Call::hangup ()
{
  if (!is_outgoing () && !IsEstablished ())
    Clear (OpalConnection::EndedByAnswerDenied);
  else
    Clear ();
}