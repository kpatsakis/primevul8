mbfl_buffer_converter_new(
    enum mbfl_no_encoding from,
    enum mbfl_no_encoding to,
    int buf_initsz)
{
	const mbfl_encoding *_from = mbfl_no2encoding(from);
	const mbfl_encoding *_to = mbfl_no2encoding(to);

	return mbfl_buffer_converter_new2(_from ? _from: &mbfl_encoding_pass, _to ? _to: &mbfl_encoding_pass, buf_initsz);
}