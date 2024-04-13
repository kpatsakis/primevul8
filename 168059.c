get_max_rate_vht_80_ss3(int mcs)
{
    switch (mcs) {
    case 0:
        return 87800000;
    case 1:
        return 175500000;
    case 2:
        return 263300000;
    case 3:
        return 351000000;
    case 4:
        return 526500000;
    case 5:
        return 702000000;
    case 6:
        return 0;
    case 7:
        return 877500000;
    case 8:
        return 105300000;
    case 9:
        return 117000000;
    }
    return 0;
}