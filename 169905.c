addBackwardPassRule(TranslationTableOffset *newRuleOffset, TranslationTableRule *newRule,
		TranslationTableHeader *table) {
	TranslationTableOffset *currentOffsetPtr;
	TranslationTableRule *currentRule;
	switch (newRule->opcode) {
	case CTO_Correct:
		currentOffsetPtr = &table->backPassRules[0];
		break;
	case CTO_Context:
		currentOffsetPtr = &table->backPassRules[1];
		break;
	case CTO_Pass2:
		currentOffsetPtr = &table->backPassRules[2];
		break;
	case CTO_Pass3:
		currentOffsetPtr = &table->backPassRules[3];
		break;
	case CTO_Pass4:
		currentOffsetPtr = &table->backPassRules[4];
		break;
	default:
		return 0;
	}
	while (*currentOffsetPtr) {
		currentRule = (TranslationTableRule *)&table->ruleArea[*currentOffsetPtr];
		if (newRule->charslen > currentRule->charslen) break;
		currentOffsetPtr = &currentRule->dotsnext;
	}
	newRule->dotsnext = *currentOffsetPtr;
	*currentOffsetPtr = *newRuleOffset;
	return 1;
}