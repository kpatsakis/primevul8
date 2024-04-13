get_max_rate_vht_160_ss3(int mcs)
{
    switch (mcs) {
    case 0:
        return 175500000;
    case 1:
        return 351000000;
    case 2:
        return 526500000;
    case 3:
        return 702000000;
    case 4:
        return 1053000000;
    case 5:
        return 1404000000;
    case 6:
        return 1579500000;
    case 7:
        return 1755000000;
    case 8:
        return 2106000000;
    case 9:
        return 0;
    }
    return 0;
}