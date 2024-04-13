get_max_rate_vht_80_ss2(int mcs)
{
    switch (mcs) {
    case 0:
        return 58500000;
    case 1:
        return 117000000;
    case 2:
        return 175500000;
    case 3:
        return 234000000;
    case 4:
        return 351000000;
    case 5:
        return 468000000;
    case 6:
        return 526500000;
    case 7:
        return 585000000;
    case 8:
        return 702000000;
    case 9:
        return 780000000;
    }
    return 0;
}