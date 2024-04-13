_lou_getCharFromDots(widechar d) {
	CharOrDots *cdPtr = getCharOrDots(d, 1, gTable);
	if (cdPtr) return cdPtr->found;
	return ' ';
}