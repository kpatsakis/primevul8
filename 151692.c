cmsBool _cmsReadWCharArray(cmsIOHANDLER* io, cmsUInt32Number n, wchar_t* Array)
{
    cmsUInt32Number i;
    cmsUInt16Number tmp;

    _cmsAssert(io != NULL);

    for (i=0; i < n; i++) {

        if (Array != NULL) {

            if (!_cmsReadUInt16Number(io, &tmp)) return FALSE;
            Array[i] = (wchar_t) tmp;
        }
        else {
            if (!_cmsReadUInt16Number(io, NULL)) return FALSE;
        }

    }
    return TRUE;
}