/* arc2sum  Mitch Withers 8/10/01

   given a hypoellipse archive phase file (.arc) read it then
   spew relevent info to stdout

   compile using cc arc2sum.c -lm -o arc2sum
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
  char arcfile[MAXSTR];
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

  char staname[6];
  char impulsive;
  char phasetype;
  char upordown;
  int pweight;
  int pyear;
  int pmonth;
  int pday;
  int phour;
  int pmin;
  float psec;
  int istadist;
  float stadist;
  int azim;
  float ssec;

  /* need all this junk to determine seh and sez */
  float x1, x2, x3, y1, y2, y3, z1, z2, z3;
  float deg2rad = 3.14159/180.0;
  float maxh, maxz, seh, sez, seo;
  float r1, r2;
  float th1, th2, th3;
  float rer1azm, rer1dip, rer2azm, rer2dip;

  if(argc != 2){
    fprintf(stderr,"usage: arc2sum <arc file>\n");
    fprintf(stderr,"\nwhere arc file is path and name to hypoe .arc file\n");

    fprintf(stderr,"\noutput format for origin line is\n");
    fprintf(stderr,"oyear, omonth, oday, ohour, omin, osec, ");
    fprintf(stderr,"lat, lon, depth, mag, npick, gap, closest, rms, Q, ");
    fprintf(stderr,"er1azm, er1dip, er1len, er2azm, er2dip, er2len, er3len");
    fprintf(stderr,"seh, sez\n");

    fprintf(stderr,"\noutput format for phase lines are\n");
    fprintf(stderr,"sta, impulsive, component, firstmotion, weight, ");
    fprintf(stderr,"year, month, day, hour, min, p-sec, dist, azim, s-sec\n");


    exit(-1);
  }

  sscanf(*(argv+1),"%s",arcfile);

  if((fid=fopen(arcfile,"r"))==NULL){
    fprintf(stderr,"ERROR: arc2sum could not open %s\n",arcfile);
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

        lon=(float)lond + ((float)lonm1 + (float)lonm2/100.0)/60.0;
        if(strncmp(&eorw,"W",1)==0)
          lon = -lon;

        sscanf(str+31,"%3d",&dep1);
        sscanf(str+34,"%2d",&dep2);
        depth = (float)dep1 + (float)dep2/100.0;

        sscanf(str+36,"%1d",&mag1);
        sscanf(str+37,"%1d",&mag2);
        mag = (float)mag1 + (float)mag2/10.0;

        sscanf(str+38,"%3d",&npick);
        sscanf(str+41,"%3d",&gap);
        sscanf(str+44,"%3d",&closest);

        sscanf(str+47,"%2d",&rms1);
        sscanf(str+49,"%2d",&rms2);
        rms = (float)rms1 + (float)rms2/100.0;

        sscanf(str+51,"%3d",&er1azm);
        sscanf(str+54,"%2d",&er1dip);
        sscanf(str+56,"%2d",&er1len1);
        sscanf(str+58,"%2d",&er1len2);
        er1len = (float)er1len1 + (float)er1len2/100.0;

        sscanf(str+60,"%3d",&er2azm);
        sscanf(str+63,"%2d",&er2dip);
        sscanf(str+65,"%2d",&er2len1);
        sscanf(str+67,"%2d",&er2len2);
        er2len = (float)er2len1 + (float)er2len2/100.0;

        sscanf(str+74,"%2d",&er3len1);
        sscanf(str+76,"%2d",&er3len2);
        er3len = (float)er3len1 + (float)er3len2/100.0;

        sscanf(str+78,"%c",&Q);

        /*********************************************************
         Convert the error ellipsoid axes to cartesian coordinates,
         then find the maximum of x and y for the MAXH error and
         find the maximum of the z's for the MAXZ error.

         MAXH and MAXZ are the horizontal and vertical projection of the joint
         hypocentral ellipsoid.  SEH and SEZ are the 68% confidence projections
         and assuming 3 degrees of freedom, (i.e. depth not fixed), are
         MAXH/1.87 and MAXZ/1.87.

         x = a sin(phi)cos(theta)
         y = b sin(phi)sin(theta)
         z = c cos(phi)

         0 < theta < 2pi  azimuth, degrees clockwise from north
         0 < phi < pi     dip, degrees down from horizontal

         To convert azimuth to the standard ccw theta, add 180 degrees
         and multiply by -1.

         Subtract 90 from the dip to convert it to the normal upward
         pointing "z" axis.

         *********************************************************/

        rer1azm = deg2rad*(float)(er1azm);
        rer2azm = deg2rad*(float)(er2azm);
        rer1dip = deg2rad*(float)(er1dip);
        rer2dip = deg2rad*(float)(er2dip);
        th1 = er1len*(float)cos((double)rer1dip);
        x1 = th1*(float)cos((double)rer1azm);
        y1 = th1*(float)sin((double)rer1azm);
        z1 = er1len*(float)sin((double)rer1dip);
        th2 = er2len*(float)cos((double)rer2dip);
        x2 = th2*(float)cos((double)rer2azm);
        y2 = th2*(float)sin((double)rer2azm);
        z2 = er2len*(float)sin((double)rer2dip);

        /*********************************************************
         scale the cross product of the first two vectors by the
         length of the third vector to get the third vector in
         cartesion coordinates i, j, k.
         a x b = (a2b3 - a3b2) i + (a3b1 - a1b3) j + (a1b2 - a2b1) k

         where a = a1i + a2j + a3k = (a1, a2, a3) and
               b = b1i + b2j + b3k = (b1, b2, b3).

         *********************************************************/
        x3 = (y1*z2 - z1*y2)*er3len/(er1len*er2len);
        y3 = (z1*x2 - x1*z2)*er3len/(er1len*er2len);
        z3 = (x1*y2 - y1*x2)*er3len/(er1len*er2len);

        th3=(float)sqrt((double)(x3*x3+y3*y3));

        th1=fabs(th1);
        th2=fabs(th2);
        th3=fabs(th3);


        maxh = th1;
        if(th2>maxh) maxh=th2;
        if(th3>maxh) maxh=th3;

        z1=fabs(z1);
        z2=fabs(z2);
        z3=fabs(z3);

        maxz = z3;
        if(z1>maxz) maxz=z1;
        if(z2>maxz) maxz=z2;

        seh = maxh/1.87;
        sez = maxz/1.87;



        fprintf(stdout,"%4.4d %2.2d %2.2d %2.2d %2.2d %5.2f ",
                        oyear,omonth,oday,ohour,omin,osec);
        fprintf(stdout,"%7.4f %8.4f ",lat,lon);
        fprintf(stdout,"%6.2f %2.1f %3d %3d %3d ",depth,mag,npick,gap,closest);
        fprintf(stdout,"%5.2f ",rms);
        fprintf(stdout,"%c ",Q);
        fprintf(stdout,"%3d %2d %5.2f ",er1azm,er1dip,er1len);
        fprintf(stdout,"%3d %2d %5.2f",er2azm,er2dip,er2len);
        fprintf(stdout," %5.2f",er3len);
        fprintf(stdout," %5.2f %5.2f\n",seh,sez);

        break;

      case 2: /* phase record */
        for(i=4; i<PHASIZE; i++) if (str[i]==' ') str[i]='0';
        sscanf(str+0,"%4s",staname);
        sscanf(str+4,"%c",&impulsive);
        sscanf(str+5,"%c",&phasetype);
        sscanf(str+6,"%c",&upordown);
        sscanf(str+7,"%1d",&pweight);

        sscanf(str+9,"%2d",&pyear);
        sscanf(str+11,"%2d",&pmonth);
        sscanf(str+13,"%2d",&pday);
        sscanf(str+15,"%2d",&phour);
        sscanf(str+17,"%2d",&pmin);

/* withers mod 20030828
        sscanf(str+19,"%2d",&isec);
        sscanf(str+22,"%2d",&imsec);
        psec = (float)isec + (float)imsec/100.0;

        sscanf(str+24,"%4.1f",&stadist);
        sscanf(str+28,"%3d",&azim);

        sscanf(str+31,"%2d",&isec);
        sscanf(str+34,"%2d",&imsec);
        ssec = (float)isec + (float)imsec/100.0;
*/
        sscanf(str+19,"%3d",&isec);
        sscanf(str+22,"%2d",&imsec);
        psec = (float)isec + (float)imsec/100.0;

        sscanf(str+24,"%4d",&istadist);
        stadist = (float)istadist/10.0;

        sscanf(str+28,"%3d",&azim);

        sscanf(str+31,"%3d",&isec);
        sscanf(str+34,"%2d",&imsec);

        ssec = (float)isec + (float)imsec/100.0;

        fprintf(stdout,"%s %c %c %c %d %d %d %d %d %d %f %f %d %f\n",
                        staname,impulsive,phasetype,upordown,pweight,
                        pyear,pmonth,pday,phour,pmin,psec,stadist,azim,ssec);

        break;
                        
    }/* end switch */

 } /* end while over endoffile */
}
