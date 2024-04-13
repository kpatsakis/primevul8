lou_free(void) {
	ChainEntry *currentEntry;
	ChainEntry *previousEntry;
	lou_logEnd();
	if (tableChain != NULL) {
		currentEntry = tableChain;
		while (currentEntry) {
			int i;
			TranslationTableHeader *t = (TranslationTableHeader *)currentEntry->table;
			for (i = 0; t->emphClasses[i]; i++) free(t->emphClasses[i]);
			free(t);
			previousEntry = currentEntry;
			currentEntry = currentEntry->next;
			free(previousEntry);
		}
		tableChain = NULL;
		lastTrans = NULL;
	}
	if (typebuf != NULL) free(typebuf);
	typebuf = NULL;
	if (wordBuffer != NULL) free(wordBuffer);
	wordBuffer = NULL;
	if (emphasisBuffer != NULL) free(emphasisBuffer);
	emphasisBuffer = NULL;
	sizeTypebuf = 0;
	if (destSpacing != NULL) free(destSpacing);
	destSpacing = NULL;
	sizeDestSpacing = 0;
	{
		int k;
		for (k = 0; k < MAXPASSBUF; k++) {
			if (passbuf[k] != NULL) free(passbuf[k]);
			passbuf[k] = NULL;
			sizePassbuf[k] = 0;
		}
	}
	if (posMapping1 != NULL) free(posMapping1);
	posMapping1 = NULL;
	sizePosMapping1 = 0;
	if (posMapping2 != NULL) free(posMapping2);
	posMapping2 = NULL;
	sizePosMapping2 = 0;
	if (posMapping3 != NULL) free(posMapping3);
	posMapping3 = NULL;
	sizePosMapping3 = 0;
	gOpcodeLengths[0] = 0;
}