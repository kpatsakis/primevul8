addBackwardRuleWithMultipleCells(widechar *cells, int count,
		TranslationTableOffset *newRuleOffset, TranslationTableRule *newRule,
		TranslationTableHeader *table) {
	/* direction = 1, newRule->dotslen > 1 */
	TranslationTableRule *currentRule = NULL;
	TranslationTableOffset *currentOffsetPtr = &table->backRules[_lou_stringHash(cells)];
	if (newRule->opcode == CTO_SwapCc) return;
	while (*currentOffsetPtr) {
		int currentLength;
		int newLength;
		currentRule = (TranslationTableRule *)&table->ruleArea[*currentOffsetPtr];
		currentLength = currentRule->dotslen + currentRule->charslen;
		newLength = count + newRule->charslen;
		if (newLength > currentLength) break;
		if (currentLength == newLength)
			if ((currentRule->opcode == CTO_Always) && (newRule->opcode != CTO_Always))
				break;
		currentOffsetPtr = &currentRule->dotsnext;
	}
	newRule->dotsnext = *currentOffsetPtr;
	*currentOffsetPtr = *newRuleOffset;
}