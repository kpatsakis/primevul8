unicode_to_char (const Unicode *unicode,
		 int      len)
{
	static UnicodeMap *uMap = nullptr;
	if (uMap == nullptr) {
		GooString *enc = new GooString("UTF-8");
		uMap = globalParams->getUnicodeMap(enc);
		uMap->incRefCnt ();
		delete enc;
	}
		
	GooString gstr;
	gchar buf[8]; /* 8 is enough for mapping an unicode char to a string */
	int i, n;

	for (i = 0; i < len; ++i) {
		n = uMap->mapUnicode(unicode[i], buf, sizeof(buf));
		gstr.append(buf, n);
	}

	return g_strdup (gstr.getCString ());
}