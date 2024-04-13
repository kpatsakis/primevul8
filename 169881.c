passGetScriptToken(CharsString *passLine, int *passLinepos, int *passPrevLinepos,
		CharsString *passHoldString, widechar *passHoldNumber, FileInfo *passNested,
		TranslationTableHeader *table) {
	while (*passLinepos < passLine->length) {
		*passPrevLinepos = *passLinepos;
		switch (passLine->chars[*passLinepos]) {
		case '\"':
			(*passLinepos)++;
			if (passGetString(passLine, passLinepos, passHoldString, passNested))
				return pass_string;
			return pass_invalidToken;
		case '@':
			(*passLinepos)++;
			if (passGetDots(passLine, passLinepos, passHoldString, passNested))
				return pass_dots;
			return pass_invalidToken;
		case '#': /* comment */
			*passLinepos = passLine->length + 1;
			return pass_noMoreTokens;
		case '!':
			if (passLine->chars[*passLinepos + 1] == '=') {
				*passLinepos += 2;
				return pass_noteq;
			}
			(*passLinepos)++;
			return pass_not;
		case '-':
			(*passLinepos)++;
			return pass_hyphen;
		case '=':
			(*passLinepos)++;
			return pass_eq;
		case '<':
			(*passLinepos)++;
			if (passLine->chars[*passLinepos] == '=') {
				(*passLinepos)++;
				return pass_lteq;
			}
			return pass_lt;
		case '>':
			(*passLinepos)++;
			if (passLine->chars[*passLinepos] == '=') {
				(*passLinepos)++;
				return pass_gteq;
			}
			return pass_gt;
		case '+':
			(*passLinepos)++;
			return pass_plus;
		case '(':
			(*passLinepos)++;
			return pass_leftParen;
		case ')':
			(*passLinepos)++;
			return pass_rightParen;
		case ',':
			(*passLinepos)++;
			return pass_comma;
		case '&':
			if (passLine->chars[ *passLinepos = 1] == '&') {
				*passLinepos += 2;
				return pass_and;
			}
			return pass_invalidToken;
		case '|':
			if (passLine->chars[*passLinepos + 1] == '|') {
				*passLinepos += 2;
				return pass_or;
			}
			return pass_invalidToken;
		case 'a':
			if (passIsKeyword("ttr", passLine, passLinepos)) return pass_attributes;
			passGetName(passLine, passLinepos, passHoldString, passNested, table);
			return pass_nameFound;
		case 'b':
			if (passIsKeyword("ack", passLine, passLinepos)) return pass_lookback;
			if (passIsKeyword("ool", passLine, passLinepos)) return pass_boolean;
			passGetName(passLine, passLinepos, passHoldString, passNested, table);
			return pass_nameFound;
		case 'c':
			if (passIsKeyword("lass", passLine, passLinepos)) return pass_class;
			passGetName(passLine, passLinepos, passHoldString, passNested, table);
			return pass_nameFound;
		case 'd':
			if (passIsKeyword("ef", passLine, passLinepos)) return pass_define;
			passGetName(passLine, passLinepos, passHoldString, passNested, table);
			return pass_nameFound;
		case 'e':
			if (passIsKeyword("mph", passLine, passLinepos)) return pass_emphasis;
			passGetName(passLine, passLinepos, passHoldString, passNested, table);
			return pass_nameFound;
		case 'f':
			if (passIsKeyword("ind", passLine, passLinepos)) return pass_search;
			if (passIsKeyword("irst", passLine, passLinepos)) return pass_first;
			passGetName(passLine, passLinepos, passHoldString, passNested, table);
			return pass_nameFound;
		case 'g':
			if (passIsKeyword("roup", passLine, passLinepos)) return pass_group;
			passGetName(passLine, passLinepos, passHoldString, passNested, table);
			return pass_nameFound;
		case 'i':
			if (passIsKeyword("f", passLine, passLinepos)) return pass_if;
			passGetName(passLine, passLinepos, passHoldString, passNested, table);
			return pass_nameFound;
		case 'l':
			if (passIsKeyword("ast", passLine, passLinepos)) return pass_last;
			passGetName(passLine, passLinepos, passHoldString, passNested, table);
			return pass_nameFound;
		case 'm':
			if (passIsKeyword("ark", passLine, passLinepos)) return pass_mark;
			passGetName(passLine, passLinepos, passHoldString, passNested, table);
			return pass_nameFound;
		case 'r':
			if (passIsKeyword("epgroup", passLine, passLinepos)) return pass_repGroup;
			if (passIsKeyword("epcopy", passLine, passLinepos)) return pass_copy;
			if (passIsKeyword("epomit", passLine, passLinepos)) return pass_omit;
			if (passIsKeyword("ep", passLine, passLinepos)) return pass_replace;
			passGetName(passLine, passLinepos, passHoldString, passNested, table);
			return pass_nameFound;
		case 's':
			if (passIsKeyword("cript", passLine, passLinepos)) return pass_script;
			if (passIsKeyword("wap", passLine, passLinepos)) return pass_swap;
			passGetName(passLine, passLinepos, passHoldString, passNested, table);
			return pass_nameFound;
		case 't':
			if (passIsKeyword("hen", passLine, passLinepos)) return pass_then;
			passGetName(passLine, passLinepos, passHoldString, passNested, table);
			return pass_nameFound;
		default:
			if (passLine->chars[*passLinepos] <= 32) {
				(*passLinepos)++;
				break;
			}
			if (passLine->chars[*passLinepos] >= '0' &&
					passLine->chars[*passLinepos] <= '9') {
				passGetNumber(passLine, passLinepos, passHoldNumber);
				return pass_numberFound;
			} else {
				if (!passGetName(
							passLine, passLinepos, passHoldString, passNested, table))
					return pass_invalidToken;
				else
					return pass_nameFound;
			}
		}
	}
	return pass_noMoreTokens;
}