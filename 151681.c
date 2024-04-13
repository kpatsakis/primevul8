void FillFirstShaper(cmsS1Fixed14Number* Table, cmsToneCurve* Curve)
{
    int i;
    cmsFloat32Number R, y;

    for (i=0; i < 256; i++) {

        R   = (cmsFloat32Number) (i / 255.0);
        y   = cmsEvalToneCurveFloat(Curve, R);

        if (y < 131072.0)
            Table[i] = DOUBLE_TO_1FIXED14(y);
        else
            Table[i] = 0x7fffffff;
    }
}