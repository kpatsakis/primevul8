_lou_getDotsForChar(widechar c) {
	CharOrDots *cdPtr = getCharOrDots(c, 0, gTable);
	if (cdPtr) return cdPtr->found;
	return B16;
}