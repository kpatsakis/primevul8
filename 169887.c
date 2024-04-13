unknownDots(widechar dots) {
	static char buffer[20];
	int k = 1;
	buffer[0] = '\\';
	for (int mappingPos = 0; dotMapping[mappingPos].key; mappingPos++) {
		if (dots & dotMapping[mappingPos].key) buffer[k++] = dotMapping[mappingPos].value;
	}
	buffer[k++] = '/';
	buffer[k] = 0;
	return buffer;
}