enum mbfl_no_encoding mbfl_encoding_detector_judge(mbfl_encoding_detector *identd)
{
	const mbfl_encoding *encoding = mbfl_encoding_detector_judge2(identd);
	return !encoding ? mbfl_no_encoding_invalid: encoding->no_encoding;
}