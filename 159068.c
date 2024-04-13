Opal::Call::OnAnswerCall (OpalConnection & connection,
			  const PString & caller)
{
  remote_party_name = (const char *) caller;

  parse_info (connection);

  return OpalCall::OnAnswerCall (connection, caller);
}