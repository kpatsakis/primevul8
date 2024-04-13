get_max_rate_vht_80_ss1(int mcs)
{
    switch (mcs) {
    case 0:
        return 29300000;
    case 1:
        return 58500000;
    case 2:
        return 87800000;
    case 3:
        return 117000000;
    case 4:
        return 175500000;
    case 5:
        return 234000000;
    case 6:
        return 263300000;
    case 7:
        return 292500000;
    case 8:
        return 351000000;
    case 9:
        return 390000000;
    }
    return 0;
}