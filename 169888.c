addCharacterClass(FileInfo *nested, const widechar *name, int length,
		CharacterClass **characterClasses,
		TranslationTableCharacterAttributes *characterClassAttribute) {
	/* Define a character class, Whether predefined or user-defined */
	CharacterClass *class;
	if (*characterClassAttribute) {
		if (!(class = malloc(sizeof(*class) + CHARSIZE * (length - 1))))
			_lou_outOfMemory();
		else {
			memset(class, 0, sizeof(*class));
			memcpy(class->name, name, CHARSIZE * (class->length = length));
			class->attribute = *characterClassAttribute;
			*characterClassAttribute <<= 1;
			class->next = *characterClasses;
			*characterClasses = class;
			return class;
		}
	}
	compileError(nested, "character class table overflow.");
	return NULL;
}