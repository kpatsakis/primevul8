onigenc_unicode_case_map(OnigCaseFoldType* flagP,
    const OnigUChar** pp, const OnigUChar* end,
    OnigUChar* to, OnigUChar* to_end,
    const struct OnigEncodingTypeST* enc)
{
  OnigCodePoint code;
  OnigUChar *to_start = to;
  OnigCaseFoldType flags = *flagP;
  int codepoint_length;

  to_end -= CASE_MAPPING_SLACK;
  /* copy flags ONIGENC_CASE_UPCASE     and ONIGENC_CASE_DOWNCASE over to
   *            ONIGENC_CASE_UP_SPECIAL and ONIGENC_CASE_DOWN_SPECIAL */
  flags |= (flags & (ONIGENC_CASE_UPCASE | ONIGENC_CASE_DOWNCASE)) << ONIGENC_CASE_SPECIAL_OFFSET;

  while (*pp < end && to <= to_end) {
    codepoint_length = ONIGENC_PRECISE_MBC_ENC_LEN(enc, *pp, end);
    if (codepoint_length < 0)
      return codepoint_length; /* encoding invalid */
    code = ONIGENC_MBC_TO_CODE(enc, *pp, end);
    *pp += codepoint_length;

    if (code <= 'z') { /* ASCII comes first */
      if (code >= 'a' && code <= 'z') {
	if (flags & ONIGENC_CASE_UPCASE) {
	  MODIFIED;
	  if (flags & ONIGENC_CASE_FOLD_TURKISH_AZERI && code == 'i')
	    code = I_WITH_DOT_ABOVE;
	  else
	    code += 'A' - 'a';
	}
      }
      else if (code >= 'A' && code <= 'Z') {
	if (flags & (ONIGENC_CASE_DOWNCASE | ONIGENC_CASE_FOLD)) {
	  MODIFIED;
	  if (flags & ONIGENC_CASE_FOLD_TURKISH_AZERI && code == 'I')
	    code = DOTLESS_i;
	  else
	    code += 'a' - 'A';
	}
      }
    }
    else if (!(flags & ONIGENC_CASE_ASCII_ONLY) && code >= 0x00B5) { /* deal with non-ASCII; micron sign (U+00B5) is lowest affected */
      const CodePointList3 *folded;

      if (code == I_WITH_DOT_ABOVE) {
	if (flags & (ONIGENC_CASE_DOWNCASE | ONIGENC_CASE_FOLD)) {
	  MODIFIED;
	  code = 'i';
	  if (!(flags & ONIGENC_CASE_FOLD_TURKISH_AZERI)) { /* make dot above explicit */
	    to += ONIGENC_CODE_TO_MBC(enc, code, to);
	    code = DOT_ABOVE;
	  }
	}
      }
      else if (code == DOTLESS_i) { /* handle this manually, because it isn't involved in folding */
	if (flags & ONIGENC_CASE_UPCASE) {
	  MODIFIED;
	  code = 'I';
	}
      }
      else if ((folded = onigenc_unicode_fold_lookup(code)) != 0) { /* data about character found in CaseFold_11_Table */
	if ((flags & ONIGENC_CASE_TITLECASE)                                 /* Titlecase needed, */
	    && (OnigCaseFoldFlags(folded->n) & ONIGENC_CASE_IS_TITLECASE)) { /* but already Titlecase  */
	  /* already Titlecase, no changes needed */
	}
	else if (flags & OnigCaseFoldFlags(folded->n)) { /* needs and data availability match */
	  const OnigCodePoint *next;
	  int count;

	  MODIFIED;
	  if (flags & OnigCaseFoldFlags(folded->n) & ONIGENC_CASE_SPECIALS) { /* special */
	    const OnigCodePoint *SpecialsStart = CaseMappingSpecials + OnigSpecialIndexDecode(folded->n);

	    if (OnigCaseFoldFlags(folded->n) & ONIGENC_CASE_IS_TITLECASE) { /* swapCASE available */
	      if ((flags & (ONIGENC_CASE_UPCASE | ONIGENC_CASE_DOWNCASE))
		  == (ONIGENC_CASE_UPCASE | ONIGENC_CASE_DOWNCASE)) /* swapCASE needed */
		goto SpecialsCopy;
	      else /* swapCASE not needed */
		SpecialsStart += SpecialsLengthExtract(*SpecialsStart);
	    }
	    if (OnigCaseFoldFlags(folded->n) & ONIGENC_CASE_TITLECASE) { /* Titlecase available */
	      if (flags & ONIGENC_CASE_TITLECASE) /* Titlecase needed, but not yet Titlecase */
		goto SpecialsCopy;
	      else /* Titlecase not needed */
		SpecialsStart += SpecialsLengthExtract(*SpecialsStart);
	    }
	    if (OnigCaseFoldFlags(folded->n) & ONIGENC_CASE_DOWN_SPECIAL) {
	      if (!(flags & ONIGENC_CASE_DOWN_SPECIAL))
		SpecialsStart += SpecialsLengthExtract(*SpecialsStart);
	    }
	    /* here, we know we use ONIGENC_CASE_UP_SPECIAL, and the position is right */
SpecialsCopy:
	    count = SpecialsLengthExtract(*SpecialsStart);
	    next = SpecialsStart;
	    code = SpecialsCodepointExtract(*next++);
	  }
	  else { /* no specials */
	    count = OnigCodePointCount(folded->n);
	    next = folded->code;
	    code = *next++;
	  }
	  if (count == 1)
	    ;
	  else if (count == 2) {
	    to += ONIGENC_CODE_TO_MBC(enc, code, to);
	    code = *next;
	  }
	  else { /* count == 3 */
	    to += ONIGENC_CODE_TO_MBC(enc, code, to);
	    to += ONIGENC_CODE_TO_MBC(enc, *next++, to);
	    code = *next;
	  }
	}
      }
      else if ((folded = onigenc_unicode_unfold1_lookup(code)) != 0  /* data about character found in CaseUnfold_11_Table */
	  && flags & OnigCaseFoldFlags(folded->n)) { /* needs and data availability match */
	MODIFIED;
	code = folded->code[(flags & OnigCaseFoldFlags(folded->n) & ONIGENC_CASE_TITLECASE) ? 1 : 0];
      }
    }
    to += ONIGENC_CODE_TO_MBC(enc, code, to);
    /* switch from titlecase to lowercase for capitalize */
    if (flags & ONIGENC_CASE_TITLECASE)
      flags ^= (ONIGENC_CASE_UPCASE | ONIGENC_CASE_DOWNCASE | ONIGENC_CASE_TITLECASE |
	  ONIGENC_CASE_UP_SPECIAL | ONIGENC_CASE_DOWN_SPECIAL);
  }
  *flagP = flags;
  return (int )(to - to_start);
}