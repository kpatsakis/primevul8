cmsBool CloseEnoughFloat(cmsFloat64Number a, cmsFloat64Number b)
{
       return fabs(b - a) < 0.00001f;
}