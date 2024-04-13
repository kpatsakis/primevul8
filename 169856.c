addForwardPassRule(TranslationTableOffset *newRuleOffset, TranslationTableRule *newRule,
		TranslationTableHeader *table) {
	TranslationTableOffset *currentOffsetPtr;
	TranslationTableRule *currentRule;
	switch (newRule->opcode) {
	case CTO_Correct:
		currentOffsetPtr = &table->forPassRules[0];
		break;
	case CTO_Context:
		currentOffsetPtr = &table->forPassRules[1];
		break;
	case CTO_Pass2:
		currentOffsetPtr = &table->forPassRules[2];
		break;
	case CTO_Pass3:
		currentOffsetPtr = &table->forPassRules[3];
		break;
	case CTO_Pass4:
		currentOffsetPtr = &table->forPassRules[4];
		break;
	default:
		return 0;
	}
	while (*currentOffsetPtr) {
		currentRule = (TranslationTableRule *)&table->ruleArea[*currentOffsetPtr];
		if (newRule->charslen > currentRule->charslen) break;
		currentOffsetPtr = &currentRule->charsnext;
	}
	newRule->charsnext = *currentOffsetPtr;
	*currentOffsetPtr = *newRuleOffset;
	return 1;
}