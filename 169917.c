setDefaults(TranslationTableHeader *table) {
	if (!table->emphRules[emph1Rule][lenPhraseOffset])
		table->emphRules[emph1Rule][lenPhraseOffset] = 4;
	if (!table->emphRules[emph2Rule][lenPhraseOffset])
		table->emphRules[emph2Rule][lenPhraseOffset] = 4;
	if (!table->emphRules[emph3Rule][lenPhraseOffset])
		table->emphRules[emph3Rule][lenPhraseOffset] = 4;
	if (table->numPasses == 0) table->numPasses = 1;
	return 1;
}