get_max_rate_ht_40(int mcs)
{
    switch (mcs) {
    case 0:
        return 13500000;
    case 1:
    case 8:
        return 27000000;
    case 2:
        return 40500000;
    case 3:
    case 9:
    case 24:
        return 54000000;
    case 4:
    case 10:
    case 17:
        return 81000000;
    case 5:
    case 11:
    case 25:
        return 108000000;
    case 6:
    case 18:
        return 121500000;
    case 7:
        return 135000000;
    case 12:
    case 19:
    case 26:
        return 162000000;
    case 13:
    case 27:
        return 216000000;
    case 14:
    case 20:
        return 243000000;
    case 15:
        return 270000000;
    case 16:
        return 40500000;
    case 21:
    case 28:
        return 324000000;
    case 22:
        return 364500000;
    case 23:
        return 405000000;
    case 29:
        return 432000000;
    case 30:
        return 486000000;
    case 31:
        return 540000000;
    }
    return 0;
}