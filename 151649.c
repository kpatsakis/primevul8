cmsBool Type_vcgt_Write(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, void* Ptr, cmsUInt32Number nItems)
{
    cmsToneCurve** Curves =  (cmsToneCurve**) Ptr;
    cmsUInt32Number i, j;

    if (cmsGetToneCurveParametricType(Curves[0]) == 5 &&
        cmsGetToneCurveParametricType(Curves[1]) == 5 &&
        cmsGetToneCurveParametricType(Curves[2]) == 5) {

            if (!_cmsWriteUInt32Number(io, cmsVideoCardGammaFormulaType)) return FALSE;

            // Save parameters
            for (i=0; i < 3; i++) {

                _cmsVCGTGAMMA v;

                v.Gamma = Curves[i] ->Segments[0].Params[0];
                v.Min   = Curves[i] ->Segments[0].Params[5];
                v.Max   = pow(Curves[i] ->Segments[0].Params[1], v.Gamma) + v.Min;

                if (!_cmsWrite15Fixed16Number(io, v.Gamma)) return FALSE;
                if (!_cmsWrite15Fixed16Number(io, v.Min)) return FALSE;
                if (!_cmsWrite15Fixed16Number(io, v.Max)) return FALSE;
            }
    }

    else {

        // Always store as a table of 256 words
        if (!_cmsWriteUInt32Number(io, cmsVideoCardGammaTableType)) return FALSE;
        if (!_cmsWriteUInt16Number(io, 3)) return FALSE;
        if (!_cmsWriteUInt16Number(io, 256)) return FALSE;
        if (!_cmsWriteUInt16Number(io, 2)) return FALSE;

        for (i=0; i < 3; i++) {
            for (j=0; j < 256; j++) {

                cmsFloat32Number v = cmsEvalToneCurveFloat(Curves[i], (cmsFloat32Number) (j / 255.0));
                cmsUInt16Number  n = _cmsQuickSaturateWord(v * 65535.0);

                if (!_cmsWriteUInt16Number(io, n)) return FALSE;
            }
        }
    }

    return TRUE;

    cmsUNUSED_PARAMETER(self);
    cmsUNUSED_PARAMETER(nItems);
}