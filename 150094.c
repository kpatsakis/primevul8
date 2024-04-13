rb_memsearch_qs_utf8_hash(const unsigned char *x)
{
    register const unsigned int mix = 8353;
    register unsigned int h = *x;
    if (h < 0xC0) {
	return h + 256;
    }
    else if (h < 0xE0) {
	h *= mix;
	h += x[1];
    }
    else if (h < 0xF0) {
	h *= mix;
	h += x[1];
	h *= mix;
	h += x[2];
    }
    else if (h < 0xF5) {
	h *= mix;
	h += x[1];
	h *= mix;
	h += x[2];
	h *= mix;
	h += x[3];
    }
    else {
	return h + 256;
    }
    return (unsigned char)h;
}