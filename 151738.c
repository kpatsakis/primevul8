cmsBool  ReadCountAndSting(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsMLU* mlu, cmsUInt32Number* SizeOfTag, const char* Section)
{
    cmsUInt32Number Count;
    char* Text;

    if (*SizeOfTag < sizeof(cmsUInt32Number)) return FALSE;

    if (!_cmsReadUInt32Number(io, &Count)) return FALSE;

    if (Count > UINT_MAX - sizeof(cmsUInt32Number)) return FALSE;
    if (*SizeOfTag < Count + sizeof(cmsUInt32Number)) return FALSE;

    Text     = (char*) _cmsMalloc(self ->ContextID, Count+1);
    if (Text == NULL) return FALSE;

    if (io ->Read(io, Text, sizeof(cmsUInt8Number), Count) != Count) {
        _cmsFree(self ->ContextID, Text);
        return FALSE;
    }

    Text[Count] = 0;

    cmsMLUsetASCII(mlu, "PS", Section, Text);
    _cmsFree(self ->ContextID, Text);

    *SizeOfTag -= (Count + sizeof(cmsUInt32Number));
    return TRUE;
}