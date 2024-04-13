cmsBool  WriteCountAndSting(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsMLU* mlu, const char* Section)
{
 cmsUInt32Number TextSize;
 char* Text;

    TextSize = cmsMLUgetASCII(mlu, "PS", Section, NULL, 0);
    Text     = (char*) _cmsMalloc(self ->ContextID, TextSize);

    if (!_cmsWriteUInt32Number(io, TextSize)) return FALSE;

    if (cmsMLUgetASCII(mlu, "PS", Section, Text, TextSize) == 0) return FALSE;

    if (!io ->Write(io, TextSize, Text)) return FALSE;
    _cmsFree(self ->ContextID, Text);

    return TRUE;
}