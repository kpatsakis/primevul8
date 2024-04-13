cmsBool _cmsWriteWCharArray(cmsIOHANDLER* io, cmsUInt32Number n, const wchar_t* Array)
{
    cmsUInt32Number i;

    _cmsAssert(io != NULL);
    _cmsAssert(!(Array == NULL && n > 0));

    for (i=0; i < n; i++) {
        if (!_cmsWriteUInt16Number(io, (cmsUInt16Number) Array[i])) return FALSE;
    }

    return TRUE;
}