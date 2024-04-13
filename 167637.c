xsltIsDigitZero(unsigned int ch)
{
    /*
     * Reference: ftp://ftp.unicode.org/Public/UNIDATA/UnicodeData.txt
     *
     * There a many more digit ranges in newer Unicode versions. These
     * are only the zeros that match Digit in XML 1.0 (IS_DIGIT macro).
     */
    switch (ch) {
    case 0x0030: case 0x0660: case 0x06F0: case 0x0966:
    case 0x09E6: case 0x0A66: case 0x0AE6: case 0x0B66:
    case 0x0C66: case 0x0CE6: case 0x0D66: case 0x0E50:
    case 0x0ED0: case 0x0F20:
	return TRUE;
    default:
	return FALSE;
    }
}