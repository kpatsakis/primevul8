uint32_t read32(uint8_t* arr, int pos,  int swapBytes)
{

    if(!swapBytes) {

        return (arr[pos]   << 24) |
               (arr[pos+1] << 16) |
               (arr[pos+2] << 8) |
                arr[pos+3];
    }

    return arr[pos] |
           (arr[pos+1] << 8) |
           (arr[pos+2] << 16) |
           (arr[pos+3] << 24);
}