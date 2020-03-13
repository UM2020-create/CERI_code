/* hyp2sum  Mitch Withers April, 2016

   20200227 update.  Doesn't correctly print info when p and s arrivals are
            on two lines. It does correctly print both lines but put bogus
            info in the fields for p when printing s and vice-versa.
            Sadly, this would require a complete rewrite. We'd need to use
            structures so that we can set pointers to null;  C doesn't allow
            a blank int or float.

   given a hypoinverse summary (.hyp) read it then
   spew relevent info to stdout

   cloned from arc2sum

   compile using cc hyp2sum.c -lm -o hyp2sum
*/

#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define FALSE 0
#define TRUE 1
#define MAXSTR 2048
#define SUMSIZE 117
#define PHASIZE 110

int main (int argc, char **argv)
{
  char hypfile[MAXSTR];
  int oyear, omonth, oday, ohour, omin;
  float osec;
  int isec, imsec;
  FILE *fid;
  int gotsum, endoffile;
  char str[MAXSTR];
  char junk;
  int i;
  int latd,latm1,latm2,lond,lonm1,lonm2;
  char nors,eorw;
  float lat,lon;
  int dep1,dep2;
  float depth;
  int mag1,mag2;
  float mag;
  int npick,gap,closest;
  int rms1,rms2;
  float rms;
  int er1azm,er1dip,er1len1,er1len2;
  float er1len;
  int er2azm,er2dip,er2len1,er2len2;
  float er2len;
  int er3len1,er3len2;
  float er3len;
  char Q;

  int cardtype;

  char staname[8];
  char network[4];
  char component[5];
  char premark[4];
  char pfirstmotion;
  int pweight;
  int pyear;
  int pmonth;
  int pday;
  int phour;
  int pmin;
  float psec;
  float presid;
  int istadist;
  float stadist;
  int azim;
  float ssec;
  char sremark[2];
  int sweight;
  float sresid;

  float seh, sez;
  int seh1, seh2, sez1, sez2;

  if(argc != 2){
    fprintf(stderr,"usage: hyp2sum <hyp file>\n");
    fprintf(stderr,"\nwhere hyp file is path and name to Hinv .hyp file\n");

    fprintf(stderr,"\noutput format for origin line is\n");
    fprintf(stderr,"oyear, omonth, oday, ohour, omin, osec ");
    fprintf(stderr,"lat, lon, depth, mag, npick, gap, closest, rms ");
    fprintf(stderr,"er1azm, er1dip, er1len, er2azm, er2dip, er2len ");
    fprintf(stderr,"er3len, seh, sez\n");

    fprintf(stderr,"\noutput format for phase lines are\n");
    fprintf(stdout,"staname, network, component, premark, pfirstmotion, pweight ");
    fprintf(stdout,"pyear, pmonth, pday, phour, pmin, psec, presid ");
    fprintf(stdout,"ssec, sremark, sweight, sresid, stadist, azim\n");

    exit(-1);
  }

  sscanf(*(argv+1),"%s",hypfile);

  if((fid=fopen(hypfile,"r"))==NULL){
    fprintf(stderr,"ERROR: hyp2sum could not open %s\n",hypfile);
    exit(-1);
  }

  endoffile=FALSE;
  while(endoffile==FALSE){
    cardtype=0;
    if( fscanf(fid,"%[^\n]",str) == EOF ){
      endoffile=TRUE;
      break;
    }else{
      if(ferror(fid)){
        fprintf(stderr,"File error\n");
        exit(-1);
      }
      fscanf(fid,"%c",&junk); /* get the new line */

      /***********************************************************************
       Determine record type 1=summary, 2=pick, 0=undefined */

      if (isdigit(str[0]) && isdigit(str[1]) && isdigit(str[2]) && \
          isdigit(str[3]) && isdigit(str[4]) && isdigit(str[5]) ) {
        cardtype=1; 
      } else if (isalnum(str[0]) && isalnum(str[1]) && isalnum(str[2]) ) {
        cardtype=2;
      } else {
        cardtype=0;
      }
    }
    switch(cardtype){
      case 1: /* summary record */
        for(i=0; i<SUMSIZE; i++) if(str[i]==' ') str[i]='0';

        sscanf(str,"%4d",&oyear);
        sscanf(str+4,"%2d",&omonth);
        sscanf(str+6,"%2d",&oday);
        sscanf(str+8,"%2d",&ohour);
        sscanf(str+10,"%2d",&omin);
        sscanf(str+12,"%2d",&isec);
        sscanf(str+14,"%2d",&imsec);
        osec = (float)isec + (float)imsec/100.0;
    
        sscanf(str+16,"%2d",&latd);
        sscanf(str+18,"%c",&nors);
        sscanf(str+19,"%2d",&latm1);
        sscanf(str+21,"%2d",&latm2);
  
        sscanf(str+23,"%3d",&lond);
        sscanf(str+26,"%c",&eorw);
        sscanf(str+27,"%2d",&lonm1);
        sscanf(str+29,"%2d",&lonm2);

        lat=(float)latd + ( (float)latm1 + (float)latm2/100.0)/60.0;
        if(strncmp(&nors,"S",1)==0)
          lat = -lat;

        /* hinv uses blanks for N and W so make the lon negative unless
           its eastern hemisphere */
        lon=(float)lond + ((float)lonm1 + (float)lonm2/100.0)/60.0;
        if(strncmp(&eorw,"E",1)!=0)
          lon = -lon;

        sscanf(str+31,"%3d",&dep1);
        sscanf(str+34,"%2d",&dep2);
        depth = (float)dep1 + (float)dep2/100.0;

        /* coda mag begins on column 71 */
        sscanf(str+70,"%1d",&mag1);
        sscanf(str+71,"%2d",&mag2);
        /* fprintf(stderr,"read mag1=%d, mag2=%d\n",mag1,mag2); */
        mag = (float)mag1 + (float)mag2/100.0;

        sscanf(str+39,"%3d",&npick);
        sscanf(str+42,"%3d",&gap);
        sscanf(str+45,"%3d",&closest);

        sscanf(str+48,"%2d",&rms1);
        sscanf(str+50,"%2d",&rms2);
        rms = (float)rms1 + (float)rms2/100.0;

        sscanf(str+52,"%3d",&er1azm);
        sscanf(str+55,"%2d",&er1dip);
        sscanf(str+57,"%2d",&er1len1);
        sscanf(str+59,"%2d",&er1len2);
        er1len = (float)er1len1 + (float)er1len2/100.0;

        sscanf(str+61,"%3d",&er2azm);
        sscanf(str+64,"%2d",&er2dip);
        sscanf(str+66,"%2d",&er2len1);
        sscanf(str+68,"%2d",&er2len2);
        er2len = (float)er2len1 + (float)er2len2/100.0;

        sscanf(str+76,"%2d",&er3len1);
        sscanf(str+78,"%2d",&er3len2);
        er3len = (float)er3len1 + (float)er3len2/100.0;

        /* hinv doesn't have the letter quality code */
        /* sscanf(str+78,"%c",&Q); */

        /* seh and sez are available in the hyp file */
        sscanf(str+76,"%2d",&er3len1);
        sscanf(str+78,"%2d",&er3len2);
        er3len = (float)er3len1 + (float)er3len2/100.0;
        sscanf(str+85,"%2d",&seh1);
        sscanf(str+87,"%2d",&seh2);
        seh = (float)seh1 + (float)seh2/100.0;
        sscanf(str+89,"%2d",&sez1);
        sscanf(str+91,"%2d",&sez2);
        sez = (float)sez1 + (float)sez2/100.0;


        fprintf(stdout,"%4.4d %2.2d %2.2d %2.2d %2.2d %5.2f ",
                        oyear,omonth,oday,ohour,omin,osec);
        fprintf(stdout,"%7.4f %8.4f ",lat,lon);
        fprintf(stdout,"%6.2f %2.1f %3d %3d %3d ",depth,mag,npick,gap,closest);
        fprintf(stdout,"%5.2f ",rms);
        fprintf(stdout,"%3d %2d %5.2f ",er1azm,er1dip,er1len);
        fprintf(stdout,"%3d %2d %5.2f",er2azm,er2dip,er2len);
        fprintf(stdout," %5.2f",er3len);
        fprintf(stdout," %5.2f %5.2f\n",seh,sez);

        break;

      case 2: /* phase record */
        for(i=16; i<47; i++) if (str[i]==' ') str[i]='0';
        for(i=48; i<(int)(strlen(str)/sizeof(char)); i++) if (str[i]==' ') str[i]='0';
        sscanf(str+0,"%5s",staname);
        sscanf(str+5,"%2s",network);
        sscanf(str+9,"%3s",component);
        sscanf(str+13,"%2s",premark);
        sscanf(str+15,"%c",&pfirstmotion);
        sscanf(str+16,"%1d",&pweight);
        fprintf(stdout,"%s %s %s %s %c %d ",
                        staname,network,component,premark,pfirstmotion,pweight);

        sscanf(str+17,"%4d",&pyear);
        sscanf(str+21,"%2d",&pmonth);
        sscanf(str+23,"%2d",&pday);
        sscanf(str+25,"%2d",&phour);
        sscanf(str+27,"%2d",&pmin);

        sscanf(str+29,"%3d",&isec);
        sscanf(str+32,"%2d",&imsec);
        psec = (float)isec + (float)imsec/100.0;

        sscanf(str+35,"%2d",&isec);
        sscanf(str+37,"%2d",&imsec);
        presid = (float)isec + (float)imsec/100.0;
        fprintf(stdout,"%d %d %d %d %d %f %f ",
                        pyear,pmonth,pday,phour,pmin,psec,presid);

        sscanf(str+41,"%3d",&isec);
        sscanf(str+44,"%2d",&imsec);
        ssec = (float)isec + (float)imsec/100.0;

        sscanf(str+46,"%2s",sremark);
        sscanf(str+49,"%1d",&sweight);

        sscanf(str+50,"%2d",&isec);
        sscanf(str+52,"%2d",&imsec);
        sresid = (float)isec + (float)imsec/100.0;

        
        sscanf(str+75,"%4d",&istadist);
        stadist = (float)istadist/10.0;

        sscanf(str+91,"%3d",&azim);

        fprintf(stdout,"%f %s %d %f %f %d\n",
                        ssec,sremark,sweight,sresid,stadist,azim);

        break;
                        
    }/* end switch */

 } /* end while over endoffile */
}
