mime_header_encoder_feed(int c, struct mime_header_encoder_data *pe)
{
	return (*pe->conv1_filter->filter_function)(c, pe->conv1_filter);
}