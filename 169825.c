deallocateCharacterClasses(CharacterClass **characterClasses) {
	while (*characterClasses) {
		CharacterClass *class = *characterClasses;
		*characterClasses = (*characterClasses)->next;
		if (class) free(class);
	}
}