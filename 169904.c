hyphenHashNew(void) {
	HyphenHashTab *hashTab;
	if (!(hashTab = malloc(sizeof(HyphenHashTab)))) _lou_outOfMemory();
	memset(hashTab, 0, sizeof(HyphenHashTab));
	return hashTab;
}