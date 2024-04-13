Opal::Call::OnReleased (OpalConnection & connection)
{
  parse_info (connection);

  OpalCall::OnReleased (connection);
}