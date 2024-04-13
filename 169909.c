passAddName(CharsString *name, int var, TranslationTableOpcode *passOpcode) {
	int k;
	struct PassName *curname;
	CharsString augmentedName;
	for (augmentedName.length = 0; augmentedName.length < name->length;
			augmentedName.length++)
		augmentedName.chars[augmentedName.length] = name->chars[augmentedName.length];
	augmentedName.chars[augmentedName.length++] = *passOpcode;
	if (!(curname = malloc(sizeof(*curname) + CHARSIZE * (augmentedName.length - 1)))) {
		_lou_outOfMemory();
	}
	memset(curname, 0, sizeof(*curname));
	for (k = 0; k < augmentedName.length; k++) {
		curname->name[k] = augmentedName.chars[k];
	}
	curname->length = augmentedName.length;
	curname->varnum = var;
	curname->next = passNames;
	passNames = curname;
	return 1;
}