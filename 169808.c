compileRule(FileInfo *nested, CharacterClass **characterClasses,
		TranslationTableCharacterAttributes *characterClassAttribute,
		short opcodeLengths[], TranslationTableOffset *newRuleOffset,
		TranslationTableRule **newRule, RuleName **ruleNames,
		TranslationTableHeader **table) {
	int lastToken = 0;
	int ok = 1;
	CharsString token;
	TranslationTableOpcode opcode;
	CharsString ruleChars;
	CharsString ruleDots;
	CharsString cells;
	CharsString scratchPad;
	CharsString emphClass;
	TranslationTableCharacterAttributes after = 0;
	TranslationTableCharacterAttributes before = 0;
	TranslationTableCharacter *c = NULL;
	widechar *patterns = NULL;
	int k, i;
	int noback, nofor;
	noback = nofor = 0;
	TranslationTableOffset tmp_offset;
doOpcode:
	if (!getToken(nested, &token, NULL, &lastToken)) return 1;	/* blank line */
	if (token.chars[0] == '#' || token.chars[0] == '<') return 1; /* comment */
	if (nested->lineNumber == 1 &&
			(eqasc2uni((unsigned char *)"ISO", token.chars, 3) ||
					eqasc2uni((unsigned char *)"UTF-8", token.chars, 5))) {
		compileHyphenation(nested, &token, &lastToken, table);
		return 1;
	}
	opcode = getOpcode(nested, &token, opcodeLengths);

	switch (opcode) { /* Carry out operations */
	case CTO_None:
		break;
	case CTO_IncludeFile: {
		CharsString includedFile;
		if (getToken(nested, &token, "include file name", &lastToken))
			if (parseChars(nested, &includedFile, &token))
				if (!includeFile(nested, &includedFile, characterClasses,
							characterClassAttribute, opcodeLengths, newRuleOffset,
							newRule, ruleNames, table))
					ok = 0;
		break;
	}
	case CTO_Locale:
		break;
	case CTO_Undefined:
		tmp_offset = (*table)->undefined;
		ok = compileBrailleIndicator(nested, "undefined character opcode", CTO_Undefined,
				&tmp_offset, &lastToken, newRuleOffset, newRule, noback, nofor, table);
		(*table)->undefined = tmp_offset;
		break;

	case CTO_Match: {
		CharsString ptn_before, ptn_after;
		TranslationTableOffset offset;
		int len, mrk;

		size_t patternsByteSize = sizeof(*patterns) * 27720;
		patterns = (widechar *)malloc(patternsByteSize);
		if (!patterns) _lou_outOfMemory();
		memset(patterns, 0xffff, patternsByteSize);

		noback = 1;
		getCharacters(nested, &ptn_before, &lastToken);
		getRuleCharsText(nested, &ruleChars, &lastToken);
		getCharacters(nested, &ptn_after, &lastToken);
		getRuleDotsPattern(nested, &ruleDots, &lastToken);

		if (!addRule(nested, opcode, &ruleChars, &ruleDots, after, before, newRuleOffset,
					newRule, noback, nofor, table))
			ok = 0;

		if (ptn_before.chars[0] == '-' && ptn_before.length == 1)
			len = _lou_pattern_compile(
					&ptn_before.chars[0], 0, &patterns[1], 13841, *table);
		else
			len = _lou_pattern_compile(
					&ptn_before.chars[0], ptn_before.length, &patterns[1], 13841, *table);
		if (!len) {
			ok = 0;
			break;
		}
		mrk = patterns[0] = len + 1;
		_lou_pattern_reverse(&patterns[1]);

		if (ptn_after.chars[0] == '-' && ptn_after.length == 1)
			len = _lou_pattern_compile(
					&ptn_after.chars[0], 0, &patterns[mrk], 13841, *table);
		else
			len = _lou_pattern_compile(
					&ptn_after.chars[0], ptn_after.length, &patterns[mrk], 13841, *table);
		if (!len) {
			ok = 0;
			break;
		}
		len += mrk;

		if (!allocateSpaceInTable(nested, &offset, len * sizeof(widechar), table)) {
			ok = 0;
			break;
		}

		/* realloc may have moved table, so make sure newRule is still valid */
		*newRule = (TranslationTableRule *)&(*table)->ruleArea[*newRuleOffset];

		memcpy(&(*table)->ruleArea[offset], patterns, len * sizeof(widechar));
		(*newRule)->patterns = offset;

		break;
	}

	case CTO_BackMatch: {
		CharsString ptn_before, ptn_after;
		TranslationTableOffset offset;
		int len, mrk;

		size_t patternsByteSize = sizeof(*patterns) * 27720;
		patterns = (widechar *)malloc(patternsByteSize);
		if (!patterns) _lou_outOfMemory();
		memset(patterns, 0xffff, patternsByteSize);

		nofor = 1;
		getCharacters(nested, &ptn_before, &lastToken);
		getRuleCharsText(nested, &ruleChars, &lastToken);
		getCharacters(nested, &ptn_after, &lastToken);
		getRuleDotsPattern(nested, &ruleDots, &lastToken);

		if (!addRule(nested, opcode, &ruleChars, &ruleDots, 0, 0, newRuleOffset, newRule,
					noback, nofor, table))
			ok = 0;

		if (ptn_before.chars[0] == '-' && ptn_before.length == 1)
			len = _lou_pattern_compile(
					&ptn_before.chars[0], 0, &patterns[1], 13841, *table);
		else
			len = _lou_pattern_compile(
					&ptn_before.chars[0], ptn_before.length, &patterns[1], 13841, *table);
		if (!len) {
			ok = 0;
			break;
		}
		mrk = patterns[0] = len + 1;
		_lou_pattern_reverse(&patterns[1]);

		if (ptn_after.chars[0] == '-' && ptn_after.length == 1)
			len = _lou_pattern_compile(
					&ptn_after.chars[0], 0, &patterns[mrk], 13841, *table);
		else
			len = _lou_pattern_compile(
					&ptn_after.chars[0], ptn_after.length, &patterns[mrk], 13841, *table);
		if (!len) {
			ok = 0;
			break;
		}
		len += mrk;

		if (!allocateSpaceInTable(nested, &offset, len * sizeof(widechar), table)) {
			ok = 0;
			break;
		}

		/* realloc may have moved table, so make sure newRule is still valid */
		*newRule = (TranslationTableRule *)&(*table)->ruleArea[*newRuleOffset];

		memcpy(&(*table)->ruleArea[offset], patterns, len * sizeof(widechar));
		(*newRule)->patterns = offset;

		break;
	}

	case CTO_BegCapsPhrase:
		tmp_offset = (*table)->emphRules[capsRule][begPhraseOffset];
		ok = compileBrailleIndicator(nested, "first word capital sign",
				CTO_BegCapsPhraseRule, &tmp_offset, &lastToken, newRuleOffset, newRule,
				noback, nofor, table);
		(*table)->emphRules[capsRule][begPhraseOffset] = tmp_offset;
		break;
	case CTO_EndCapsPhrase:
		switch (compileBeforeAfter(nested, &lastToken)) {
		case 1:  // before
			if ((*table)->emphRules[capsRule][endPhraseAfterOffset]) {
				compileError(nested, "Capital sign after last word already defined.");
				ok = 0;
				break;
			}
			tmp_offset = (*table)->emphRules[capsRule][endPhraseBeforeOffset];
			ok = compileBrailleIndicator(nested, "capital sign before last word",
					CTO_EndCapsPhraseBeforeRule, &tmp_offset, &lastToken, newRuleOffset,
					newRule, noback, nofor, table);
			(*table)->emphRules[capsRule][endPhraseBeforeOffset] = tmp_offset;
			break;
		case 2:  // after
			if ((*table)->emphRules[capsRule][endPhraseBeforeOffset]) {
				compileError(nested, "Capital sign before last word already defined.");
				ok = 0;
				break;
			}
			tmp_offset = (*table)->emphRules[capsRule][endPhraseAfterOffset];
			ok = compileBrailleIndicator(nested, "capital sign after last word",
					CTO_EndCapsPhraseAfterRule, &tmp_offset, &lastToken, newRuleOffset,
					newRule, noback, nofor, table);
			(*table)->emphRules[capsRule][endPhraseAfterOffset] = tmp_offset;
			break;
		default:  // error
			compileError(nested, "Invalid lastword indicator location.");
			ok = 0;
			break;
		}
		break;
	case CTO_BegCaps:
		tmp_offset = (*table)->emphRules[capsRule][begOffset];
		ok = compileBrailleIndicator(nested, "first letter capital sign", CTO_BegCapsRule,
				&tmp_offset, &lastToken, newRuleOffset, newRule, noback, nofor, table);
		(*table)->emphRules[capsRule][begOffset] = tmp_offset;
		break;
	case CTO_EndCaps:
		tmp_offset = (*table)->emphRules[capsRule][endOffset];
		ok = compileBrailleIndicator(nested, "last letter capital sign", CTO_EndCapsRule,
				&tmp_offset, &lastToken, newRuleOffset, newRule, noback, nofor, table);
		(*table)->emphRules[capsRule][endOffset] = tmp_offset;
		break;
	case CTO_CapsLetter:
		tmp_offset = (*table)->emphRules[capsRule][letterOffset];
		ok = compileBrailleIndicator(nested, "single letter capital sign",
				CTO_CapsLetterRule, &tmp_offset, &lastToken, newRuleOffset, newRule,
				noback, nofor, table);
		(*table)->emphRules[capsRule][letterOffset] = tmp_offset;
		break;
	case CTO_BegCapsWord:
		tmp_offset = (*table)->emphRules[capsRule][begWordOffset];
		ok = compileBrailleIndicator(nested, "capital word", CTO_BegCapsWordRule,
				&tmp_offset, &lastToken, newRuleOffset, newRule, noback, nofor, table);
		(*table)->emphRules[capsRule][begWordOffset] = tmp_offset;
		break;
	case CTO_EndCapsWord:
		tmp_offset = (*table)->emphRules[capsRule][endWordOffset];
		ok = compileBrailleIndicator(nested, "capital word stop", CTO_EndCapsWordRule,
				&tmp_offset, &lastToken, newRuleOffset, newRule, noback, nofor, table);
		(*table)->emphRules[capsRule][endWordOffset] = tmp_offset;
		break;
	case CTO_LenCapsPhrase:
		ok = (*table)->emphRules[capsRule][lenPhraseOffset] =
				compileNumber(nested, &lastToken);
		break;

	/* these 9 general purpose emphasis opcodes are compiled further down to more specific
	 * internal opcodes:
	 * - emphletter
	 * - begemphword
	 * - endemphword
	 * - begemph
	 * - endemph
	 * - begemphphrase
	 * - endemphphrase
	 * - lenemphphrase
	 */
	case CTO_EmphClass:
		if (getToken(nested, &token, "emphasis class", &lastToken))
			if (parseChars(nested, &emphClass, &token)) {
				char *s = malloc(sizeof(char) * (emphClass.length + 1));
				for (k = 0; k < emphClass.length; k++) s[k] = (char)emphClass.chars[k];
				s[k++] = '\0';
				for (i = 0; (*table)->emphClasses[i]; i++)
					if (strcmp(s, (*table)->emphClasses[i]) == 0) {
						_lou_logMessage(LOG_WARN, "Duplicate emphasis class: %s", s);
						warningCount++;
						free(s);
						return 1;
					}
				if (i < MAX_EMPH_CLASSES) {
					switch (i) {
					/* For backwards compatibility (i.e. because programs will assume the
					 * first 3
					 * typeform bits are `italic', `underline' and `bold') we require that
					 * the first
					 * 3 emphclass definitions are (in that order):
					 *
					 *   emphclass italic
					 *   emphclass underline
					 *   emphclass bold
					 *
					 * While it would be possible to use the emphclass opcode only for
					 * defining
					 * _additional_ classes (not allowing for them to be called italic,
					 * underline or
					 * bold), thereby reducing the amount of boilerplate, we deliberately
					 * choose not
					 * to do that in order to not give italic, underline and bold any
					 * special
					 * status. The hope is that eventually all programs will use liblouis
					 * for
					 * emphasis the recommended way (i.e. by looking up the supported
					 * typeforms in
					 * the documentation or API) so that we can drop this restriction.
					 */
					case 0:
						if (strcmp(s, "italic") != 0) {
							_lou_logMessage(LOG_ERROR,
									"First emphasis class must be \"italic\" but got %s",
									s);
							errorCount++;
							free(s);
							return 0;
						}
						break;
					case 1:
						if (strcmp(s, "underline") != 0) {
							_lou_logMessage(LOG_ERROR,
									"Second emphasis class must be \"underline\" but got "
									"%s",
									s);
							errorCount++;
							free(s);
							return 0;
						}
						break;
					case 2:
						if (strcmp(s, "bold") != 0) {
							_lou_logMessage(LOG_ERROR,
									"Third emphasis class must be \"bold\" but got %s",
									s);
							errorCount++;
							free(s);
							return 0;
						}
						break;
					}
					(*table)->emphClasses[i] = s;
					(*table)->emphClasses[i + 1] = NULL;
					ok = 1;
					break;
				} else {
					_lou_logMessage(LOG_ERROR,
							"Max number of emphasis classes (%i) reached",
							MAX_EMPH_CLASSES);
					errorCount++;
					free(s);
					ok = 0;
					break;
				}
			}
		compileError(nested, "emphclass must be followed by a valid class name.");
		ok = 0;
		break;
	case CTO_EmphLetter:
	case CTO_BegEmphWord:
	case CTO_EndEmphWord:
	case CTO_BegEmph:
	case CTO_EndEmph:
	case CTO_BegEmphPhrase:
	case CTO_EndEmphPhrase:
	case CTO_LenEmphPhrase:
		ok = 0;
		if (getToken(nested, &token, "emphasis class", &lastToken))
			if (parseChars(nested, &emphClass, &token)) {
				char *s = malloc(sizeof(char) * (emphClass.length + 1));
				for (k = 0; k < emphClass.length; k++) s[k] = (char)emphClass.chars[k];
				s[k++] = '\0';
				for (i = 0; (*table)->emphClasses[i]; i++)
					if (strcmp(s, (*table)->emphClasses[i]) == 0) break;
				if (!(*table)->emphClasses[i]) {
					_lou_logMessage(LOG_ERROR, "Emphasis class %s not declared", s);
					errorCount++;
					free(s);
					break;
				}
				i++;  // in table->emphRules the first index is used for caps
				if (opcode == CTO_EmphLetter) {
					tmp_offset = (*table)->emphRules[i][letterOffset];
					ok = compileBrailleIndicator(nested, "single letter",
							CTO_Emph1LetterRule + letterOffset + (8 * i), &tmp_offset,
							&lastToken, newRuleOffset, newRule, noback, nofor, table);
					(*table)->emphRules[i][letterOffset] = tmp_offset;
				} else if (opcode == CTO_BegEmphWord) {
					tmp_offset = (*table)->emphRules[i][begWordOffset];
					ok = compileBrailleIndicator(nested, "word",
							CTO_Emph1LetterRule + begWordOffset + (8 * i), &tmp_offset,
							&lastToken, newRuleOffset, newRule, noback, nofor, table);
					(*table)->emphRules[i][begWordOffset] = tmp_offset;
				} else if (opcode == CTO_EndEmphWord) {
					tmp_offset = (*table)->emphRules[i][endWordOffset];
					ok = compileBrailleIndicator(nested, "word stop",
							CTO_Emph1LetterRule + endWordOffset + (8 * i), &tmp_offset,
							&lastToken, newRuleOffset, newRule, noback, nofor, table);
					(*table)->emphRules[i][endWordOffset] = tmp_offset;
				} else if (opcode == CTO_BegEmph) {
					/* fail if both begemph and any of begemphphrase or begemphword are
					 * defined */
					if ((*table)->emphRules[i][begWordOffset] ||
							(*table)->emphRules[i][begPhraseOffset]) {
						compileError(nested,
								"Cannot define emphasis for both no context and word or "
								"phrase context, i.e. cannot have both begemph and "
								"begemphword or begemphphrase.");
						ok = 0;
						break;
					}
					tmp_offset = (*table)->emphRules[i][begOffset];
					ok = compileBrailleIndicator(nested, "first letter",
							CTO_Emph1LetterRule + begOffset + (8 * i), &tmp_offset,
							&lastToken, newRuleOffset, newRule, noback, nofor, table);
					(*table)->emphRules[i][begOffset] = tmp_offset;
				} else if (opcode == CTO_EndEmph) {
					if ((*table)->emphRules[i][endWordOffset] ||
							(*table)->emphRules[i][endPhraseBeforeOffset] ||
							(*table)->emphRules[i][endPhraseAfterOffset]) {
						compileError(nested,
								"Cannot define emphasis for both no context and word or "
								"phrase context, i.e. cannot have both endemph and "
								"endemphword or endemphphrase.");
						ok = 0;
						break;
					}
					tmp_offset = (*table)->emphRules[i][endOffset];
					ok = compileBrailleIndicator(nested, "last letter",
							CTO_Emph1LetterRule + endOffset + (8 * i), &tmp_offset,
							&lastToken, newRuleOffset, newRule, noback, nofor, table);
					(*table)->emphRules[i][endOffset] = tmp_offset;
				} else if (opcode == CTO_BegEmphPhrase) {
					tmp_offset = (*table)->emphRules[i][begPhraseOffset];
					ok = compileBrailleIndicator(nested, "first word",
							CTO_Emph1LetterRule + begPhraseOffset + (8 * i), &tmp_offset,
							&lastToken, newRuleOffset, newRule, noback, nofor, table);
					(*table)->emphRules[i][begPhraseOffset] = tmp_offset;
				} else if (opcode == CTO_EndEmphPhrase)
					switch (compileBeforeAfter(nested, &lastToken)) {
					case 1:  // before
						if ((*table)->emphRules[i][endPhraseAfterOffset]) {
							compileError(nested, "last word after already defined.");
							ok = 0;
							break;
						}
						tmp_offset = (*table)->emphRules[i][endPhraseBeforeOffset];
						ok = compileBrailleIndicator(nested, "last word before",
								CTO_Emph1LetterRule + endPhraseBeforeOffset + (8 * i),
								&tmp_offset, &lastToken, newRuleOffset, newRule, noback,
								nofor, table);
						(*table)->emphRules[i][endPhraseBeforeOffset] = tmp_offset;
						break;
					case 2:  // after
						if ((*table)->emphRules[i][endPhraseBeforeOffset]) {
							compileError(nested, "last word before already defined.");
							ok = 0;
							break;
						}
						tmp_offset = (*table)->emphRules[i][endPhraseAfterOffset];
						ok = compileBrailleIndicator(nested, "last word after",
								CTO_Emph1LetterRule + endPhraseAfterOffset + (8 * i),
								&tmp_offset, &lastToken, newRuleOffset, newRule, noback,
								nofor, table);
						(*table)->emphRules[i][endPhraseAfterOffset] = tmp_offset;
						break;
					default:  // error
						compileError(nested, "Invalid lastword indicator location.");
						ok = 0;
						break;
					}
				else if (opcode == CTO_LenEmphPhrase)
					ok = (*table)->emphRules[i][lenPhraseOffset] =
							compileNumber(nested, &lastToken);
				free(s);
			}
		break;

	case CTO_LetterSign:
		tmp_offset = (*table)->letterSign;
		ok = compileBrailleIndicator(nested, "letter sign", CTO_LetterRule, &tmp_offset,
				&lastToken, newRuleOffset, newRule, noback, nofor, table);
		(*table)->letterSign = tmp_offset;
		break;
	case CTO_NoLetsignBefore:
		if (getRuleCharsText(nested, &ruleChars, &lastToken)) {
			if (((*table)->noLetsignBeforeCount + ruleChars.length) > LETSIGNSIZE) {
				compileError(nested, "More than %d characters", LETSIGNSIZE);
				ok = 0;
				break;
			}
			for (k = 0; k < ruleChars.length; k++)
				(*table)->noLetsignBefore[(*table)->noLetsignBeforeCount++] =
						ruleChars.chars[k];
		}
		break;
	case CTO_NoLetsign:
		if (getRuleCharsText(nested, &ruleChars, &lastToken)) {
			if (((*table)->noLetsignCount + ruleChars.length) > LETSIGNSIZE) {
				compileError(nested, "More than %d characters", LETSIGNSIZE);
				ok = 0;
				break;
			}
			for (k = 0; k < ruleChars.length; k++)
				(*table)->noLetsign[(*table)->noLetsignCount++] = ruleChars.chars[k];
		}
		break;
	case CTO_NoLetsignAfter:
		if (getRuleCharsText(nested, &ruleChars, &lastToken)) {
			if (((*table)->noLetsignAfterCount + ruleChars.length) > LETSIGNSIZE) {
				compileError(nested, "More than %d characters", LETSIGNSIZE);
				ok = 0;
				break;
			}
			for (k = 0; k < ruleChars.length; k++)
				(*table)->noLetsignAfter[(*table)->noLetsignAfterCount++] =
						ruleChars.chars[k];
		}
		break;
	case CTO_NumberSign:
		tmp_offset = (*table)->numberSign;
		ok = compileBrailleIndicator(nested, "number sign", CTO_NumberRule, &tmp_offset,
				&lastToken, newRuleOffset, newRule, noback, nofor, table);
		(*table)->numberSign = tmp_offset;
		break;

	case CTO_Attribute:

		c = NULL;
		ok = 1;
		if (!getToken(nested, &ruleChars, "attribute number", &lastToken)) {
			compileError(nested, "Expected attribute number.");
			ok = 0;
			break;
		}

		k = -1;
		switch (ruleChars.chars[0]) {
		case '0':
			k = 0;
			break;
		case '1':
			k = 1;
			break;
		case '2':
			k = 2;
			break;
		case '3':
			k = 3;
			break;
		case '4':
			k = 4;
			break;
		case '5':
			k = 5;
			break;
		case '6':
			k = 6;
			break;
		case '7':
			k = 7;
			break;
		}
		if (k == -1) {
			compileError(nested, "Invalid attribute number.");
			ok = 0;
			break;
		}

		if (getRuleCharsText(nested, &ruleChars, &lastToken)) {
			for (i = 0; i < ruleChars.length; i++) {
				c = compile_findCharOrDots(ruleChars.chars[i], 0, *table);
				if (c)
					c->attributes |= (CTC_UserDefined0 << k);
				else {
					compileError(nested, "Attribute character undefined");
					ok = 0;
					break;
				}
			}
		}
		break;

	case CTO_NumericModeChars:

		c = NULL;
		ok = 1;
		if (getRuleCharsText(nested, &ruleChars, &lastToken)) {
			for (k = 0; k < ruleChars.length; k++) {
				c = compile_findCharOrDots(ruleChars.chars[k], 0, *table);
				if (c)
					c->attributes |= CTC_NumericMode;
				else {
					compileError(nested, "Numeric mode character undefined");
					ok = 0;
					break;
				}
			}
			(*table)->usesNumericMode = 1;
		}
		break;

	case CTO_NumericNoContractChars:

		c = NULL;
		ok = 1;
		if (getRuleCharsText(nested, &ruleChars, &lastToken)) {
			for (k = 0; k < ruleChars.length; k++) {
				c = compile_findCharOrDots(ruleChars.chars[k], 0, *table);
				if (c)
					c->attributes |= CTC_NumericNoContract;
				else {
					compileError(nested, "Numeric no contraction character undefined");
					ok = 0;
					break;
				}
			}
			(*table)->usesNumericMode = 1;
		}
		break;

	case CTO_NoContractSign:

		tmp_offset = (*table)->noContractSign;
		ok = compileBrailleIndicator(nested, "no contractions sign", CTO_NoContractRule,
				&tmp_offset, &lastToken, newRuleOffset, newRule, noback, nofor, table);
		(*table)->noContractSign = tmp_offset;
		break;

	case CTO_SeqDelimiter:

		c = NULL;
		ok = 1;
		if (getRuleCharsText(nested, &ruleChars, &lastToken)) {
			for (k = 0; k < ruleChars.length; k++) {
				c = compile_findCharOrDots(ruleChars.chars[k], 0, *table);
				if (c)
					c->attributes |= CTC_SeqDelimiter;
				else {
					compileError(nested, "Sequence delimiter character undefined");
					ok = 0;
					break;
				}
			}
			(*table)->usesSequences = 1;
		}
		break;

	case CTO_SeqBeforeChars:

		c = NULL;
		ok = 1;
		if (getRuleCharsText(nested, &ruleChars, &lastToken)) {
			for (k = 0; k < ruleChars.length; k++) {
				c = compile_findCharOrDots(ruleChars.chars[k], 0, *table);
				if (c)
					c->attributes |= CTC_SeqBefore;
				else {
					compileError(nested, "Sequence before character undefined");
					ok = 0;
					break;
				}
			}
		}
		break;

	case CTO_SeqAfterChars:

		c = NULL;
		ok = 1;
		if (getRuleCharsText(nested, &ruleChars, &lastToken)) {
			for (k = 0; k < ruleChars.length; k++) {
				c = compile_findCharOrDots(ruleChars.chars[k], 0, *table);
				if (c)
					c->attributes |= CTC_SeqAfter;
				else {
					compileError(nested, "Sequence after character undefined");
					ok = 0;
					break;
				}
			}
		}
		break;

	case CTO_SeqAfterPattern:

		if (getRuleCharsText(nested, &ruleChars, &lastToken)) {
			if (((*table)->seqPatternsCount + ruleChars.length + 1) > SEQPATTERNSIZE) {
				compileError(nested, "More than %d characters", SEQPATTERNSIZE);
				ok = 0;
				break;
			}
			for (k = 0; k < ruleChars.length; k++)
				(*table)->seqPatterns[(*table)->seqPatternsCount++] = ruleChars.chars[k];
			(*table)->seqPatterns[(*table)->seqPatternsCount++] = 0;
		}
		break;
	case CTO_SeqAfterExpression:

		if (getRuleCharsText(nested, &ruleChars, &lastToken)) {
			for ((*table)->seqAfterExpressionLength = 0;
					(*table)->seqAfterExpressionLength < ruleChars.length;
					(*table)->seqAfterExpressionLength++)
				(*table)->seqAfterExpression[(*table)->seqAfterExpressionLength] =
						ruleChars.chars[(*table)->seqAfterExpressionLength];
			(*table)->seqAfterExpression[(*table)->seqAfterExpressionLength] = 0;
		}
		break;

	case CTO_CapsModeChars:

		c = NULL;
		ok = 1;
		if (getRuleCharsText(nested, &ruleChars, &lastToken)) {
			for (k = 0; k < ruleChars.length; k++) {
				c = compile_findCharOrDots(ruleChars.chars[k], 0, *table);
				if (c)
					c->attributes |= CTC_CapsMode;
				else {
					compileError(nested, "Capital mode character undefined");
					ok = 0;
					break;
				}
			}
		}
		break;

	case CTO_BegComp:
		tmp_offset = (*table)->begComp;
		ok = compileBrailleIndicator(nested, "begin computer braille", CTO_BegCompRule,
				&tmp_offset, &lastToken, newRuleOffset, newRule, noback, nofor, table);
		(*table)->begComp = tmp_offset;
		break;
	case CTO_EndComp:
		tmp_offset = (*table)->endComp;
		ok = compileBrailleIndicator(nested, "end computer braslle", CTO_EndCompRule,
				&tmp_offset, &lastToken, newRuleOffset, newRule, noback, nofor, table);
		(*table)->endComp = tmp_offset;
		break;
	case CTO_Syllable:
		(*table)->syllables = 1;
	case CTO_Always:
	case CTO_NoCross:
	case CTO_LargeSign:
	case CTO_WholeWord:
	case CTO_PartWord:
	case CTO_JoinNum:
	case CTO_JoinableWord:
	case CTO_LowWord:
	case CTO_SuffixableWord:
	case CTO_PrefixableWord:
	case CTO_BegWord:
	case CTO_BegMidWord:
	case CTO_MidWord:
	case CTO_MidEndWord:
	case CTO_EndWord:
	case CTO_PrePunc:
	case CTO_PostPunc:
	case CTO_BegNum:
	case CTO_MidNum:
	case CTO_EndNum:
	case CTO_Repeated:
	case CTO_RepWord:
		if (getRuleCharsText(nested, &ruleChars, &lastToken))
			if (getRuleDotsPattern(nested, &ruleDots, &lastToken))
				if (!addRule(nested, opcode, &ruleChars, &ruleDots, after, before,
							newRuleOffset, newRule, noback, nofor, table))
					ok = 0;
		// if (opcode == CTO_MidNum)
		// {
		//   TranslationTableCharacter *c = compile_findCharOrDots(ruleChars.chars[0], 0);
		//   if(c)
		//     c->attributes |= CTC_NumericMode;
		// }
		break;
	case CTO_CompDots:
	case CTO_Comp6:
		if (!getRuleCharsText(nested, &ruleChars, &lastToken)) return 0;
		if (ruleChars.length != 1 || ruleChars.chars[0] > 255) {
			compileError(nested, "first operand must be 1 character and < 256");
			return 0;
		}
		if (!getRuleDotsPattern(nested, &ruleDots, &lastToken)) return 0;
		if (!addRule(nested, opcode, &ruleChars, &ruleDots, after, before, newRuleOffset,
					newRule, noback, nofor, table))
			ok = 0;
		(*table)->compdotsPattern[ruleChars.chars[0]] = *newRuleOffset;
		break;
	case CTO_ExactDots:
		if (!getRuleCharsText(nested, &ruleChars, &lastToken)) return 0;
		if (ruleChars.chars[0] != '@') {
			compileError(nested, "The operand must begin with an at sign (@)");
			return 0;
		}
		for (k = 1; k < ruleChars.length; k++)
			scratchPad.chars[k - 1] = ruleChars.chars[k];
		scratchPad.length = ruleChars.length - 1;
		if (!parseDots(nested, &ruleDots, &scratchPad)) return 0;
		if (!addRule(nested, opcode, &ruleChars, &ruleDots, before, after, newRuleOffset,
					newRule, noback, nofor, table))
			ok = 0;
		break;
	case CTO_CapsNoCont:
		ruleChars.length = 1;
		ruleChars.chars[0] = 'a';
		if (!addRule(nested, CTO_CapsNoContRule, &ruleChars, NULL, after, before,
					newRuleOffset, newRule, noback, nofor, table))
			ok = 0;
		(*table)->capsNoCont = *newRuleOffset;
		break;
	case CTO_Replace:
		if (getRuleCharsText(nested, &ruleChars, &lastToken)) {
			if (lastToken)
				ruleDots.length = ruleDots.chars[0] = 0;
			else {
				getRuleDotsText(nested, &ruleDots, &lastToken);
				if (ruleDots.chars[0] == '#')
					ruleDots.length = ruleDots.chars[0] = 0;
				else if (ruleDots.chars[0] == '\\' && ruleDots.chars[1] == '#')
					memcpy(&ruleDots.chars[0], &ruleDots.chars[1],
							ruleDots.length-- * CHARSIZE);
			}
		}
		for (k = 0; k < ruleChars.length; k++)
			addCharOrDots(nested, ruleChars.chars[k], 0, table);
		for (k = 0; k < ruleDots.length; k++)
			addCharOrDots(nested, ruleDots.chars[k], 0, table);
		if (!addRule(nested, opcode, &ruleChars, &ruleDots, after, before, newRuleOffset,
					newRule, noback, nofor, table))
			ok = 0;
		break;
	case CTO_Correct:
		(*table)->corrections = 1;
		goto doPass;
	case CTO_Pass2:
		if ((*table)->numPasses < 2) (*table)->numPasses = 2;
		goto doPass;
	case CTO_Pass3:
		if ((*table)->numPasses < 3) (*table)->numPasses = 3;
		goto doPass;
	case CTO_Pass4:
		if ((*table)->numPasses < 4) (*table)->numPasses = 4;
	doPass:
	case CTO_Context:
		if (!(nofor || noback)) {
			compileError(nested, "%s or %s must be specified.",
					_lou_findOpcodeName(CTO_NoFor), _lou_findOpcodeName(CTO_NoBack));
			ok = 0;
			break;
		}
		if (!compilePassOpcode(nested, opcode, *characterClasses, newRuleOffset, newRule,
					noback, nofor, *ruleNames, table))
			ok = 0;
		break;
	case CTO_Contraction:
	case CTO_NoCont:
	case CTO_CompBrl:
	case CTO_Literal:
		if (getRuleCharsText(nested, &ruleChars, &lastToken))
			if (!addRule(nested, opcode, &ruleChars, NULL, after, before, newRuleOffset,
						newRule, noback, nofor, table))
				ok = 0;
		break;
	case CTO_MultInd: {
		int t;
		ruleChars.length = 0;
		if (getToken(nested, &token, "multiple braille indicators", &lastToken) &&
				parseDots(nested, &cells, &token)) {
			while ((t = getToken(nested, &token, "multind opcodes", &lastToken))) {
				opcode = getOpcode(nested, &token, opcodeLengths);
				if (opcode >= CTO_CapsLetter && opcode < CTO_MultInd)
					ruleChars.chars[ruleChars.length++] = (widechar)opcode;
				else {
					compileError(nested, "Not a braille indicator opcode.");
					ok = 0;
				}
				if (t == 2) break;
			}
		} else
			ok = 0;
		if (!addRule(nested, CTO_MultInd, &ruleChars, &cells, after, before,
					newRuleOffset, newRule, noback, nofor, table))
			ok = 0;
		break;
	}

	case CTO_Class: {
		CharsString characters;
		const CharacterClass *class;
		if (!*characterClasses) {
			if (!allocateCharacterClasses(characterClasses, characterClassAttribute))
				ok = 0;
		}
		if (getToken(nested, &token, "character class name", &lastToken)) {
			class = findCharacterClass(&token, *characterClasses);
			if (!class)
				// no class with that name: create one
				class = addCharacterClass(nested, &token.chars[0], token.length,
						characterClasses, characterClassAttribute);
			if (class) {
				// there is a class with that name or a new class was successfully created
				if (getCharacters(nested, &characters, &lastToken)) {
					int index;
					for (index = 0; index < characters.length; ++index) {
						TranslationTableRule *defRule;
						TranslationTableCharacter *character = definedCharOrDots(
								nested, characters.chars[index], 0, *table);
						character->attributes |= class->attribute;
						defRule = (TranslationTableRule *)&(
								*table)->ruleArea[character->definitionRule];
						if (defRule->dotslen == 1) {
							character = definedCharOrDots(nested,
									defRule->charsdots[defRule->charslen], 1, *table);
							character->attributes |= class->attribute;
						}
					}
				}
			}
		}
		break;
	}

		{
			TranslationTableCharacterAttributes *attributes;
			const CharacterClass *class;
		case CTO_After:
			attributes = &after;
			goto doClass;
		case CTO_Before:
			attributes = &before;
		doClass:

			if (!*characterClasses) {
				if (!allocateCharacterClasses(characterClasses, characterClassAttribute))
					ok = 0;
			}
			if (getCharacterClass(nested, &class, *characterClasses, &lastToken)) {
				*attributes |= class->attribute;
				goto doOpcode;
			}
			break;
		}

	case CTO_NoBack:
		if (nofor) {
			compileError(nested, "%s already specified.", _lou_findOpcodeName(CTO_NoFor));
			ok = 0;
			break;
		}
		noback = 1;
		goto doOpcode;
	case CTO_NoFor:
		if (noback) {
			compileError(
					nested, "%s already specified.", _lou_findOpcodeName(CTO_NoBack));
			ok = 0;
			break;
		}
		nofor = 1;
		goto doOpcode;

	case CTO_EmpMatchBefore:
		before |= CTC_EmpMatch;
		goto doOpcode;
	case CTO_EmpMatchAfter:
		after |= CTC_EmpMatch;
		goto doOpcode;

	case CTO_SwapCc:
	case CTO_SwapCd:
	case CTO_SwapDd:
		if (!compileSwap(nested, opcode, &lastToken, newRuleOffset, newRule, noback,
					nofor, ruleNames, table))
			ok = 0;
		break;
	case CTO_Hyphen:
	case CTO_DecPoint:
		//	case CTO_Apostrophe:
		//	case CTO_Initial:
		if (getRuleCharsText(nested, &ruleChars, &lastToken))
			if (getRuleDotsPattern(nested, &ruleDots, &lastToken)) {
				if (ruleChars.length != 1 || ruleDots.length < 1) {
					compileError(nested,
							"One Unicode character and at least one cell are required.");
					ok = 0;
				}
				if (!addRule(nested, opcode, &ruleChars, &ruleDots, after, before,
							newRuleOffset, newRule, noback, nofor, table))
					ok = 0;
				// if (opcode == CTO_DecPoint)
				// {
				//   TranslationTableCharacter *c =
				//   compile_findCharOrDots(ruleChars.chars[0], 0);
				//   if(c)
				//     c->attributes |= CTC_NumericMode;
				// }
			}
		break;
	case CTO_Space:
		compileCharDef(nested, opcode, CTC_Space, &lastToken, newRuleOffset, newRule,
				noback, nofor, table);
		break;
	case CTO_Digit:
		compileCharDef(nested, opcode, CTC_Digit, &lastToken, newRuleOffset, newRule,
				noback, nofor, table);
		break;
	case CTO_LitDigit:
		compileCharDef(nested, opcode, CTC_LitDigit, &lastToken, newRuleOffset, newRule,
				noback, nofor, table);
		break;
	case CTO_Punctuation:
		compileCharDef(nested, opcode, CTC_Punctuation, &lastToken, newRuleOffset,
				newRule, noback, nofor, table);
		break;
	case CTO_Math:
		compileCharDef(nested, opcode, CTC_Math, &lastToken, newRuleOffset, newRule,
				noback, nofor, table);
		break;
	case CTO_Sign:
		compileCharDef(nested, opcode, CTC_Sign, &lastToken, newRuleOffset, newRule,
				noback, nofor, table);
		break;
	case CTO_Letter:
		compileCharDef(nested, opcode, CTC_Letter, &lastToken, newRuleOffset, newRule,
				noback, nofor, table);
		break;
	case CTO_UpperCase:
		compileCharDef(nested, opcode, CTC_UpperCase, &lastToken, newRuleOffset, newRule,
				noback, nofor, table);
		break;
	case CTO_LowerCase:
		compileCharDef(nested, opcode, CTC_LowerCase, &lastToken, newRuleOffset, newRule,
				noback, nofor, table);
		break;
	case CTO_Grouping:
		ok = compileGrouping(nested, &lastToken, newRuleOffset, newRule, noback, nofor,
				ruleNames, table);
		break;
	case CTO_UpLow:
		ok = compileUplow(
				nested, &lastToken, newRuleOffset, newRule, noback, nofor, table);
		break;
	case CTO_Display:
		if (getRuleCharsText(nested, &ruleChars, &lastToken))
			if (getRuleDotsPattern(nested, &ruleDots, &lastToken)) {
				if (ruleChars.length != 1 || ruleDots.length != 1) {
					compileError(
							nested, "Exactly one character and one cell are required.");
					ok = 0;
				}
				putCharAndDots(nested, ruleChars.chars[0], ruleDots.chars[0], table);
			}
		break;
	default:
		compileError(nested, "unimplemented opcode.");
		ok = 0;
		break;
	}

	if (patterns != NULL) free(patterns);

	return ok;
}