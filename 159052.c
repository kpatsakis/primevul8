void Opal::Call::set_reject_delay (unsigned delay)
{
  NoAnswerTimer.SetInterval (0, PMIN (delay, 60));
}