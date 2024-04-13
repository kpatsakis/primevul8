uint16_t read16(uint8_t* arr, int pos,  int swapBytes)
{
    uint8_t b1 = arr[pos];
    uint8_t b2 = arr[pos+1];

    return (swapBytes) ?  ((b2 << 8) | b1) : ((b1 << 8) | b2);
}