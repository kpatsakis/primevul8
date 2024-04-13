allocateCharacterClasses(CharacterClass **characterClasses,
		TranslationTableCharacterAttributes *characterClassAttribute) {
	/* Allocate memory for predifined character classes */
	int k = 0;
	*characterClasses = NULL;
	*characterClassAttribute = 1;
	while (characterClassNames[k]) {
		widechar wname[MAXSTRING];
		int length = (int)strlen(characterClassNames[k]);
		int kk;
		for (kk = 0; kk < length; kk++) wname[kk] = (widechar)characterClassNames[k][kk];
		if (!addCharacterClass(
					NULL, wname, length, characterClasses, characterClassAttribute)) {
			deallocateCharacterClasses(characterClasses);
			return 0;
		}
		k++;
	}
	return 1;
}