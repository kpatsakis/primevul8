cmsBool  Type_Text_Description_Write(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, void* Ptr, cmsUInt32Number nItems)
{
    cmsMLU* mlu = (cmsMLU*) Ptr;
    char *Text = NULL;
    wchar_t *Wide = NULL;
    cmsUInt32Number len, len_text, len_tag_requirement, len_aligned;
    cmsBool  rc = FALSE;
    char Filler[68];

    // Used below for writing zeroes
    memset(Filler, 0, sizeof(Filler));

    // Get the len of string
    len = cmsMLUgetASCII(mlu, cmsNoLanguage, cmsNoCountry, NULL, 0);

    // Specification ICC.1:2001-04 (v2.4.0): It has been found that textDescriptionType can contain misaligned data
    //(see clause 4.1 for the definition of aligned). Because the Unicode language
    // code and Unicode count immediately follow the ASCII description, their
    // alignment is not correct if the ASCII count is not a multiple of four. The
    // ScriptCode code is misaligned when the ASCII count is odd. Profile reading and
    // writing software must be written carefully in order to handle these alignment
    // problems.
    //
    // The above last sentence suggest to handle alignment issues in the
    // parser. The provided example (Table 69 on Page 60) makes this clear. 
    // The padding only in the ASCII count is not sufficient for a aligned tag
    // size, with the same text size in ASCII and Unicode.

    // Null strings
    if (len <= 0) {

        Text = (char*)    _cmsDupMem(self ->ContextID, "", sizeof(char));
        Wide = (wchar_t*) _cmsDupMem(self ->ContextID, L"", sizeof(wchar_t));
    }
    else {
        // Create independent buffers
        Text = (char*) _cmsCalloc(self ->ContextID, len, sizeof(char));
        if (Text == NULL) goto Error;

        Wide = (wchar_t*) _cmsCalloc(self ->ContextID, len, sizeof(wchar_t));
        if (Wide == NULL) goto Error;

        // Get both representations.
        cmsMLUgetASCII(mlu, cmsNoLanguage, cmsNoCountry,  Text, len * sizeof(char));
        cmsMLUgetWide(mlu,  cmsNoLanguage, cmsNoCountry,  Wide, len * sizeof(wchar_t));
    }

    // Tell the real text len including the null terminator and padding
    len_text = (cmsUInt32Number) strlen(Text) + 1;
    // Compute an total tag size requirement
    len_tag_requirement = (8+4+len_text+4+4+2*len_text+2+1+67);
    len_aligned = _cmsALIGNLONG(len_tag_requirement);

  // * cmsUInt32Number       count;          * Description length
  // * cmsInt8Number         desc[count]     * NULL terminated ascii string
  // * cmsUInt32Number       ucLangCode;     * UniCode language code
  // * cmsUInt32Number       ucCount;        * UniCode description length
  // * cmsInt16Number        ucDesc[ucCount];* The UniCode description
  // * cmsUInt16Number       scCode;         * ScriptCode code
  // * cmsUInt8Number        scCount;        * ScriptCode count
  // * cmsInt8Number         scDesc[67];     * ScriptCode Description

    if (!_cmsWriteUInt32Number(io, len_text)) goto Error;
    if (!io ->Write(io, len_text, Text)) goto Error;

    if (!_cmsWriteUInt32Number(io, 0)) goto Error;  // ucLanguageCode

    if (!_cmsWriteUInt32Number(io, len_text)) goto Error;
    // Note that in some compilers sizeof(cmsUInt16Number) != sizeof(wchar_t)
    if (!_cmsWriteWCharArray(io, len_text, Wide)) goto Error;

    // ScriptCode Code & count (unused)
    if (!_cmsWriteUInt16Number(io, 0)) goto Error;
    if (!_cmsWriteUInt8Number(io, 0)) goto Error;

    if (!io ->Write(io, 67, Filler)) goto Error;

    // possibly add pad at the end of tag
    if(len_aligned - len_tag_requirement > 0)
      if (!io ->Write(io, len_aligned - len_tag_requirement, Filler)) goto Error;

    rc = TRUE;

Error:
    if (Text) _cmsFree(self ->ContextID, Text);
    if (Wide) _cmsFree(self ->ContextID, Wide);

    return rc;

    cmsUNUSED_PARAMETER(nItems);
}