void *Type_S15Fixed16_Read(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt32Number* nItems, cmsUInt32Number SizeOfTag)
{
    cmsFloat64Number*  array_double;
    cmsUInt32Number i, n;

    *nItems = 0;
    n = SizeOfTag / sizeof(cmsUInt32Number);
    array_double = (cmsFloat64Number*) _cmsCalloc(self ->ContextID, n, sizeof(cmsFloat64Number));
    if (array_double == NULL) return NULL;

    for (i=0; i < n; i++) {

        if (!_cmsRead15Fixed16Number(io, &array_double[i])) {

            _cmsFree(self ->ContextID, array_double);
            return NULL;
        }
    }

    *nItems = n;
    return (void*) array_double;
}