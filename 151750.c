void FillSecondShaper(cmsUInt16Number* Table, cmsToneCurve* Curve, cmsBool Is8BitsOutput)
{
    int i;
    cmsFloat32Number R, Val;

    for (i=0; i < 16385; i++) {

        R   = (cmsFloat32Number) (i / 16384.0);
        Val = cmsEvalToneCurveFloat(Curve, R);    // Val comes 0..1.0

        if (Val < 0)
            Val = 0;

        if (Val > 1.0)
            Val = 1.0;

        if (Is8BitsOutput) {

            // If 8 bits output, we can optimize further by computing the / 257 part.
            // first we compute the resulting byte and then we store the byte times
            // 257. This quantization allows to round very quick by doing a >> 8, but
            // since the low byte is always equal to msb, we can do a & 0xff and this works!
            cmsUInt16Number w = _cmsQuickSaturateWord(Val * 65535.0);
            cmsUInt8Number  b = FROM_16_TO_8(w);

            Table[i] = FROM_8_TO_16(b);
        }
        else Table[i]  = _cmsQuickSaturateWord(Val * 65535.0);
    }
}