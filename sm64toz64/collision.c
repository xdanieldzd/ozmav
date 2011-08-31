/* collision.c - spinout Aug 2010
 based off of JSA's collision fixer 3.2 (written in FreeBasic), using
 MNGoldenEagle's algorithm to calculate collision normals. 
*/
#include <math.h>

#define U16(a,p)	(((a)[p]<<8)|((a)[p+1]))
#define S16(a,p)	(signed short)U16(a,p)

void FixCollision(unsigned char * data, int vertOff, int TriOff, int TriCount)
{
    int i, pos, end = TriOff + (TriCount << 4);
    int v1, v2, v3, p1[3], p2[3], p3[3], dx[2], dy[2], dz[2], dn, ni[3];
    float nf[3], nd;
    for(pos=TriOff;pos<end;pos+=0x10)
    {
        v1 = U16(data, pos + 2);
        v2 = U16(data, pos + 4);
        v3 = U16(data, pos + 6);
        for(i=0;i<3;i++)
        {
            p1[i] = S16(data, vertOff + (v1 * 0x6) + (i << 1));
            p2[i] = S16(data, vertOff + (v2 * 0x6) + (i << 1));
            p3[i] = S16(data, vertOff + (v3 * 0x6) + (i << 1));
        }
        dx[0] = p1[0] - p2[0]; dx[1] = p2[0] - p3[0];
        dy[0] = p1[1] - p2[1]; dy[1] = p2[1] - p3[1];
        dz[0] = p1[2] - p2[2]; dz[1] = p2[2] - p3[2];
        
        ni[0] = (dy[0] * dz[1]) - (dz[0] * dy[1]);
        ni[1] = (dz[0] * dx[1]) - (dx[0] * dz[1]);
        ni[2] = (dx[0] * dy[1]) - (dy[0] * dx[1]);
        
        
        for(i=0;i<3;i++)
            nf[i] = (float)ni[i] * ni[i];
        
        nd = nf[0] + nf[1] + nf[2];
        
        for(i=0;i<3;i++)
        {
            if (nd)
                nf[i] /= nd;
            nf[i] *= 0x3FFF0001;
            nf[i] = sqrt(nf[i]);
            if(ni[i] < 0)
                nf[i] *= -1;
        }
        
        dn = (int)( (nf[0] * p1[0] / 0x7FFF) + (nf[1] * p1[1] / 0x7FFF) + (nf[2] * p1[2] / 0x7FFF) ) * -1;
        
        if(dn < 0)
            dn+=0x10000;
        data[pos+0xE] = (dn>>8)&0xFF;
        data[pos+0xF] = dn&0xFF;
        dn &= 0xFFFF;
        
        for(i=0;i<3;i++)
        {
            ni[i] = (int)nf[i];
            if(ni[i] < 0)
                ni[i] += 0x10000;
            ni[i] &= 0xFFFF;
            data[pos+8+(i<<1)] = (ni[i]>>8)&0xFF;
            data[pos+9+(i<<1)] = ni[i]&0xFF;
        }
    }
}
