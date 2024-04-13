getCharacterClass(FileInfo *nested, const CharacterClass **class,
		CharacterClass *characterClasses, int *lastToken) {
	CharsString token;
	if (getToken(nested, &token, "character class name", lastToken)) {
		if ((*class = findCharacterClass(&token, characterClasses))) return 1;
		compileError(nested, "character class not defined.");
	}
	return 0;
}