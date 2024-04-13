mime_header_decoder_feed(int c, struct mime_header_decoder_data *pd)
{
	return mime_header_decoder_collector(c, pd);
}