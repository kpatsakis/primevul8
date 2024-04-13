static void snd_usb_audio_stream_free(struct snd_usb_stream *stream)
{
	free_substream(&stream->substream[0]);
	free_substream(&stream->substream[1]);
	list_del(&stream->list);
	kfree(stream);
}