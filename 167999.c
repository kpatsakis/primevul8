get_max_rate_vht_160_ss2(int mcs)
{
    switch (mcs) {
    case 0:
        return 117000000;
    case 1:
        return 234000000;
    case 2:
        return 351000000;
    case 3:
        return 468000000;
    case 4:
        return 702000000;
    case 5:
        return 936000000;
    case 6:
        return 1053000000;
    case 7:
        return 1170000000;
    case 8:
        return 1404000000;
    case 9:
        return 1560000000;
    }
    return 0;
}