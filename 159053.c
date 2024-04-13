Opal::Call::OnOpenMediaStream (OpalMediaStream & stream)
{
  StreamType type = (stream.GetMediaFormat().GetMediaType() == OpalMediaType::Audio ()) ? Audio : Video;
  bool is_transmitting = false;
  std::string stream_name;

  stream_name = std::string ((const char *) stream.GetMediaFormat ().GetEncodingName ());
  std::transform (stream_name.begin (), stream_name.end (), stream_name.begin (), (int (*) (int)) toupper);
  is_transmitting = !stream.IsSource ();

  Ekiga::Runtime::run_in_main (boost::bind (boost::ref (stream_opened), stream_name, type, is_transmitting));
}