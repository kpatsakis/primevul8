void LibRaw::nikon_load_sraw()
{
  // We're already seeked to data!
  unsigned char *rd = (unsigned char *)malloc(3*(imgdata.sizes.raw_width+2));
  if(!rd) throw LIBRAW_EXCEPTION_ALLOC;
  try {
    int row,col;
    for(row = 0; row < imgdata.sizes.raw_height; row++)
      {
        checkCancel();
        libraw_internal_data.internal_data.input->read(rd,3,imgdata.sizes.raw_width);
        for(col = 0; col < imgdata.sizes.raw_width-1;col+=2)
          {
            int bi = col*3;
            ushort bits1 = (rd[bi+1] &0xf)<<8| rd[bi]; // 3,0,1
            ushort bits2 = rd[bi+2] << 4 | ((rd[bi+1]>>4)& 0xf); //452
            ushort bits3 =  ((rd[bi+4] & 0xf)<<8) | rd[bi+3]; // 967
            ushort bits4 = rd[bi+5] << 4 | ((rd[bi+4]>>4)& 0xf); // ab8
            imgdata.image[row*imgdata.sizes.raw_width+col][0]=bits1;
            imgdata.image[row*imgdata.sizes.raw_width+col][1]=bits3;
            imgdata.image[row*imgdata.sizes.raw_width+col][2]=bits4;
            imgdata.image[row*imgdata.sizes.raw_width+col+1][0]=bits2;
            imgdata.image[row*imgdata.sizes.raw_width+col+1][1]=2048;
            imgdata.image[row*imgdata.sizes.raw_width+col+1][2]=2048;
          }
      }
  }catch (...) {
    free(rd);
    throw ;
  }
  free(rd);
  C.maximum = 0xfff; // 12 bit?
  if(imgdata.params.sraw_ycc>=2)
    {
      return; // no CbCr interpolation
    }
  // Interpolate CC channels
  int row,col;
  for(row = 0; row < imgdata.sizes.raw_height; row++)
    {
      checkCancel(); // will throw out
      for(col = 0; col < imgdata.sizes.raw_width;col+=2)
        {
          int col2 = col<imgdata.sizes.raw_width-2?col+2:col;
          imgdata.image[row*imgdata.sizes.raw_width+col+1][1]
            =(unsigned short)(int(imgdata.image[row*imgdata.sizes.raw_width+col][1]
                                  +imgdata.image[row*imgdata.sizes.raw_width+col2][1])/2);
          imgdata.image[row*imgdata.sizes.raw_width+col+1][2]
            =(unsigned short)(int(imgdata.image[row*imgdata.sizes.raw_width+col][2]
                                  +imgdata.image[row*imgdata.sizes.raw_width+col2][2])/2);
        }
    }
  if(imgdata.params.sraw_ycc>0)
    return;

  for(row = 0; row < imgdata.sizes.raw_height; row++)
    {
      checkCancel(); // will throw out
      for(col = 0; col < imgdata.sizes.raw_width;col++)
        {
          float Y = float(imgdata.image[row*imgdata.sizes.raw_width+col][0])/2549.f;
          float Ch2 = float(imgdata.image[row*imgdata.sizes.raw_width+col][1]-1280)/1536.f;
          float Ch3 = float(imgdata.image[row*imgdata.sizes.raw_width+col][2]-1280)/1536.f;
          if(Y>1.f) Y = 1.f;
		  if(Y>0.803f) Ch2 = Ch3 = 0.5f;
          float r = Y + 1.40200f*(Ch3 - 0.5f);
		  if(r<0.f) r=0.f;
		  if(r>1.f) r=1.f;
          float g = Y - 0.34414f*(Ch2-0.5f) - 0.71414*(Ch3 - 0.5f) ;
		  if(g>1.f) g = 1.f;
		  if(g<0.f) g = 0.f;
          float b = Y + 1.77200*(Ch2-0.5f);
		  if(b>1.f) b = 1.f;
		  if(b<0.f) b = 0.f;
          imgdata.image[row*imgdata.sizes.raw_width+col][0]=imgdata.color.curve[int(r*3072.f)];
          imgdata.image[row*imgdata.sizes.raw_width+col][1]=imgdata.color.curve[int(g*3072.f)];
          imgdata.image[row*imgdata.sizes.raw_width+col][2]=imgdata.color.curve[int(b*3072.f)];
        }
    }
  C.maximum=16383;
}