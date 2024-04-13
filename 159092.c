Opal::Call::toggle_stream_pause (StreamType type)
{
  OpalMediaStreamPtr stream = NULL;
  PString codec_name;
  std::string stream_name;

  bool paused = false;

  PSafePtr<OpalConnection> connection = get_remote_connection ();
  if (connection != NULL) {

    stream = connection->GetMediaStream ((type == Audio) ? OpalMediaType::Audio () : OpalMediaType::Video (), false);
    if (stream != NULL) {

      stream_name = std::string ((const char *) stream->GetMediaFormat ().GetEncodingName ());
      std::transform (stream_name.begin (), stream_name.end (), stream_name.begin (), (int (*) (int)) toupper);
      paused = stream->IsPaused ();
      stream->SetPaused (!paused);

      if (paused)
	Ekiga::Runtime::run_in_main (boost::bind (boost::ref (stream_resumed), stream_name, type));
      else
	Ekiga::Runtime::run_in_main (boost::bind (boost::ref (stream_paused), stream_name, type));
    }
  }
}