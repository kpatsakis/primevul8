passGetNumber(CharsString *passLine, int *passLinepos, widechar *passHoldNumber) {
	/* Convert a string of wide character digits to an integer */
	*passHoldNumber = 0;
	while ((*passLinepos < passLine->length) && (passLine->chars[*passLinepos] >= '0') &&
			(passLine->chars[*passLinepos] <= '9'))
		*passHoldNumber =
				10 * (*passHoldNumber) + (passLine->chars[(*passLinepos)++] - '0');
	return 1;
}