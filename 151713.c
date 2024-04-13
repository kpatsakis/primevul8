cmsBool WriteOneWChar(cmsIOHANDLER* io,  _cmsDICelem* e, cmsUInt32Number i, const wchar_t * wcstr, cmsUInt32Number BaseOffset)
{
    cmsUInt32Number Before = io ->Tell(io);
    cmsUInt32Number n;

    e ->Offsets[i] = Before - BaseOffset;

    if (wcstr == NULL) {
        e ->Sizes[i] = 0;
        e ->Offsets[i] = 0;
        return TRUE;
    }

    n = mywcslen(wcstr);
    if (!_cmsWriteWCharArray(io,  n, wcstr)) return FALSE;

    e ->Sizes[i] = io ->Tell(io) - Before;
    return TRUE;
}