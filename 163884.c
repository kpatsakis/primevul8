cmsBool HandlePhotoshopAPP13(jpeg_saved_marker_ptr ptr)
{
    while (ptr) {

        if (ptr -> marker == (JPEG_APP0 + 13) && ptr -> data_length > 9)
        {
            JOCTET FAR* data = ptr -> data;

            if(GETJOCTET(data[0]) == 0x50 &&
               GETJOCTET(data[1]) == 0x68 &&
               GETJOCTET(data[2]) == 0x6F &&
               GETJOCTET(data[3]) == 0x74 &&
               GETJOCTET(data[4]) == 0x6F &&
               GETJOCTET(data[5]) == 0x73 &&
               GETJOCTET(data[6]) == 0x68 &&
               GETJOCTET(data[7]) == 0x6F &&
               GETJOCTET(data[8]) == 0x70) {

                ProcessPhotoshopAPP13(data, ptr -> data_length);
                return TRUE;
            }
        }

        ptr = ptr -> next;
    }

    return FALSE;
}