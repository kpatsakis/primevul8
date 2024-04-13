cmsUInt32Number uipow(cmsUInt32Number n, cmsUInt32Number a, cmsUInt32Number b)
{
    cmsUInt32Number rv = 1, rc;

    if (a == 0) return 0;
    if (n == 0) return 0;

    for (; b > 0; b--) {

        rv *= a;

        // Check for overflow
        if (rv > UINT_MAX / a) return (cmsUInt32Number) -1;

    }

    rc = rv * n;

    if (rv != rc / n) return (cmsUInt32Number) -1;
    return rc;
}