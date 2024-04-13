void Opal::Call::set_no_answer_forward (unsigned delay, const std::string & uri)
{
  forward_uri = uri;

  NoAnswerTimer.SetInterval (0, PMIN (delay, 60));
}