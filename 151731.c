void *Type_UcrBg_Read(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt32Number* nItems, cmsUInt32Number SizeOfTag)
{
    cmsUcrBg* n = (cmsUcrBg*) _cmsMallocZero(self ->ContextID, sizeof(cmsUcrBg));
    cmsUInt32Number CountUcr, CountBg;
    char* ASCIIString;

    *nItems = 0;
    if (n == NULL) return NULL;

    // First curve is Under color removal
    if (!_cmsReadUInt32Number(io, &CountUcr)) return NULL;
    if (SizeOfTag < sizeof(cmsUInt32Number)) return NULL;
    SizeOfTag -= sizeof(cmsUInt32Number);

    n ->Ucr = cmsBuildTabulatedToneCurve16(self ->ContextID, CountUcr, NULL);
    if (n ->Ucr == NULL) return NULL;

    if (!_cmsReadUInt16Array(io, CountUcr, n ->Ucr->Table16)) return NULL;
    if (SizeOfTag < sizeof(cmsUInt32Number)) return NULL;
    SizeOfTag -= CountUcr * sizeof(cmsUInt16Number);

    // Second curve is Black generation
    if (!_cmsReadUInt32Number(io, &CountBg)) return NULL;
    if (SizeOfTag < sizeof(cmsUInt32Number)) return NULL;
    SizeOfTag -= sizeof(cmsUInt32Number);

    n ->Bg = cmsBuildTabulatedToneCurve16(self ->ContextID, CountBg, NULL);
    if (n ->Bg == NULL) return NULL;
    if (!_cmsReadUInt16Array(io, CountBg, n ->Bg->Table16)) return NULL;
    if (SizeOfTag < CountBg * sizeof(cmsUInt16Number)) return NULL;
    SizeOfTag -= CountBg * sizeof(cmsUInt16Number);
    if (SizeOfTag == UINT_MAX) return NULL;

    // Now comes the text. The length is specified by the tag size
    n ->Desc = cmsMLUalloc(self ->ContextID, 1);
    if (n ->Desc == NULL) return NULL;

    ASCIIString = (char*) _cmsMalloc(self ->ContextID, SizeOfTag + 1);
    if (io ->Read(io, ASCIIString, sizeof(char), SizeOfTag) != SizeOfTag) return NULL;
    ASCIIString[SizeOfTag] = 0;
    cmsMLUsetASCII(n ->Desc, cmsNoLanguage, cmsNoCountry, ASCIIString);
    _cmsFree(self ->ContextID, ASCIIString);

    *nItems = 1;
    return (void*) n;
}