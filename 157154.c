static void snd_usb_audio_pcm_free(struct snd_pcm *pcm)
{
	struct snd_usb_stream *stream = pcm->private_data;
	if (stream) {
		stream->pcm = NULL;
		snd_usb_audio_stream_free(stream);
	}
}