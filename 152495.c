void LibRaw::adjust_bl()
{
  int clear_repeat=0;
   if (O.user_black >= 0)
     {
       C.black = O.user_black;
       clear_repeat = 1;
     }
   for(int i=0; i<4; i++)
     if(O.user_cblack[i]>-1000000)
       {
         C.cblack[i] = O.user_cblack[i];
         clear_repeat  = 1;
       }

   if(clear_repeat)
     C.cblack[4]=C.cblack[5]=0;

 // Add common part to cblack[] early
   if (imgdata.idata.filters > 1000 && (C.cblack[4]+1)/2 == 1 && (C.cblack[5]+1)/2 == 1)
   {
	   for(int c=0; c<4; c++)
		   C.cblack[c] += C.cblack[6 + c/2 % C.cblack[4] * C.cblack[5] + c%2 % C.cblack[5]];
	   C.cblack[4]=C.cblack[5]=0;
   }
   else if(imgdata.idata.filters <= 1000 && C.cblack[4]==1 && C.cblack[5]==1) // Fuji RAF dng
   {
	   for(int c=0; c<4; c++)
		   C.cblack[c] += C.cblack[6];
	   C.cblack[4]=C.cblack[5]=0;
   }
  // remove common part from C.cblack[]
  int i = C.cblack[3];
  int c;
  for(c=0;c<3;c++) if (i > C.cblack[c]) i = C.cblack[c];

  for(c=0;c<4;c++) C.cblack[c] -= i; // remove common part
  C.black += i;

  // Now calculate common part for cblack[6+] part and move it to C.black

  if(C.cblack[4] && C.cblack[5])
    {
      i = C.cblack[6];
      for(c=1; c<C.cblack[4]*C.cblack[5]; c++)
        if(i>C.cblack[6+c]) i = C.cblack[6+c];
      // Remove i from cblack[6+]
      int nonz=0;
      for(c=0; c<C.cblack[4]*C.cblack[5]; c++)
        {
          C.cblack[6+c]-=i;
          if(C.cblack[6+c])nonz++;
        }
      C.black +=i;
      if(!nonz)
        C.cblack[4] = C.cblack[5] = 0;
    }
  for(c=0;c<4;c++) C.cblack[c] += C.black;
}