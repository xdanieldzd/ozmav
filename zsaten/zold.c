/* zsaten/ozmav2: zold.c
 read old format limb structures
 spinout 2010
 */

#include "globals.h"

#define U32(x)	( ((x)[0]<<24)|((x)[1]<<16)|((x)[2]<<8)|(x)[3] )
#define U16(x)	( ((x)[0]<<8)|(x)[1] )
#define S32(x)	((signed)U32(x))
#define S16(x)	((signed)U16(x))
union {u32 u; f32 f;} ____t;
#define F32(x)	( ( (____t.u = U32(x)) & 0) + ____t.f)

/* Linked list entry... */
typedef struct __z_old_limb {
    u32 dl;
    f32 xyz[3];
    s16 rot_d[3];
    s16 rot[3];
    u16 __align;
    struct __z_old_limb *sibling;
    struct __z_old_limb *child;
    int n;
} z_old_limb;


/* must have PERFECT data */
int get_old_limb_n(u32 offset, u32 order[])
{
    int i=0;
    while(order[i] != offset) i++;
    return i;
}

z_old_limb *
read_z_old_limb(u32 offset, u32 order[])
{
    z_old_limb *l;
    u32 sibling, child;
    u8 *data;
    
    if(!RDP_CheckAddressValidity(offset))
        return NULL;
    
    l = malloc(sizeof(z_old_limb));
    l->n	= get_old_limb_n(offset, order);
    
    data = RAM[offset>>24].Data + (offset & 0xFFFFFF);
    
    l->dl	= U32(data);
    l->xyz[0]	= F32(data+0x4);
    l->xyz[1]	= F32(data+0x8);
    l->xyz[2]	= F32(data+0xC);
    l->rot_d[0]	= S16(data + 0x10);
    l->rot_d[1]	= S16(data + 0x12);
    l->rot_d[2]	= S16(data + 0x14);
    sibling	= U32(data + 0x18);
    child	= U32(data + 0x1C);
    l->sibling	= (RDP_CheckAddressValidity(sibling)) ? read_z_old_limb(sibling, order) : NULL;
    l->child	= (RDP_CheckAddressValidity(child)) ? read_z_old_limb(child, order) : NULL;
    
    return l;
}

z_old_limb *
old_limb_tree_search(z_old_limb *l, int n)
{
    z_old_limb *r=NULL;
    if(l->n == n)
        return l;
    if(l->sibling != NULL)
        r = old_limb_tree_search(l->sibling, n);
    if(r == NULL && l->child != NULL)
        r = old_limb_tree_search(l->child, n);
    if(r)
        return r;
    return NULL;
}

void
old_limb_free_tree(z_old_limb *l)
{
    if(l->sibling != NULL)
        old_limb_free_tree(l->sibling);
    if(l->child != NULL)
        old_limb_free_tree(l->child);
    free(l);
}

void
processOldObject(u32 first)
{
    vCurrentActor.offsetBoneSetup[0] = first;
    vCurrentActor.boneSetupTotal = 0;
    vCurrentActor.actorScale = 0.01;
    scanOldAnimations(0x6);
    vCurrentActor.animTotal++;
    vCurrentActor.offsetAnims[vCurrentActor.animTotal] = 0xDEADBEEF;
    vCurrentActor.old_limb_top = NULL;
}

int scanOldAnimations(u8 bank)
{    vCurrentActor.animTotal = -1;
    int i;    for(i = 0; i < RAM[bank].Size; i += 4) {        if ((!RAM[bank].Data[i])	&&            (RAM[bank].Data[i+1] > 0)	&&            (!RAM[bank].Data[i+2])	&&            (RAM[bank].Data[i+3] > 0)	&&
            RDP_CheckAddressValidity(U32(RAM[bank].Data+i+4)) &&
            RDP_CheckAddressValidity(U32(RAM[bank].Data+i+8)) &&
            U32(RAM[bank].Data+i+4) < U32(RAM[bank].Data+i+8) &&
            !(U32(RAM[bank].Data+i+4)&0xF)
           ){            	vCurrentActor.animTotal++;            	vCurrentActor.offsetAnims[vCurrentActor.animTotal] = (bank << 24) | i;            }    }
    return 0;
}

void drawOldBone(z_old_limb *limb)
{
    if(vProgram.showBones) {
        // borrowed from UoT
        if(RDP_OpenGL_ExtFragmentProgram()) glDisable(GL_FRAGMENT_PROGRAM_ARB);
        glDisable(GL_TEXTURE);
        glDisable(GL_LIGHTING);
        glDisable(GL_NORMALIZE);

        glDepthRange(0, 0);
        glLineWidth(4);
        glBegin(GL_LINES);
        glColor3f(1.0f, 0.0f, 1.0f);
        glVertex3d(0, 0, 0);
        glVertex3d((int)limb->xyz[0], (int)limb->xyz[1], (int)limb->xyz[2]);
        glEnd();
        glPointSize(6);
        glBegin(GL_POINTS);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex3d((int)limb->xyz[0], (int)limb->xyz[1], (int)limb->xyz[2]);
        glEnd();
        glPointSize(3);
        glBegin(GL_POINTS);
        glColor3f(vBoneColorFactor.R, vBoneColorFactor.G, vBoneColorFactor.B);
        glVertex3d((int)limb->xyz[0], (int)limb->xyz[1], (int)limb->xyz[2]);
        glEnd();
        glPointSize(1);
        glLineWidth(1);
        glDepthRange(0, 1);

        glEnable(GL_TEXTURE);
        glEnable(GL_LIGHTING);
        glEnable(GL_NORMALIZE);
    }

    glPushMatrix();

    glTranslated((int)limb->xyz[0], (int)limb->xyz[1], (int)limb->xyz[2]);
    glRotated((int)limb->rot[2] / 182.0444444, 0, 0, 1);
    glRotated((int)limb->rot[1] / 182.0444444, 0, 1, 0);
    glRotated((int)limb->rot[0] / 182.0444444, 1, 0, 0);

    //Draw display list
    if(limb->dl && RDP_CheckAddressValidity(limb->dl)){
        RDP_ClearStructures(false);
        RDP_ParseDisplayList(limb->dl, true);
    }

    //Draw child
    if(limb->child != NULL) {
        vBoneColorFactor.R = 1.0f;
        vBoneColorFactor.G = 0.0f;
        vBoneColorFactor.B = 0.0f;
        drawOldBone(limb->child);
    } else {
        vBoneColorFactor.R = 1.0f;
        vBoneColorFactor.G = 1.0f;
        vBoneColorFactor.B = 1.0f;
    }

    glPopMatrix();

    //Draw sibling/next bone
    if(limb->sibling != NULL) {
        vBoneColorFactor.R = 0.0f;
        vBoneColorFactor.G = 0.0f;
        vBoneColorFactor.B = 1.0f;
        drawOldBone(limb->sibling);
    } else {
        vBoneColorFactor.R = 1.0f;
        vBoneColorFactor.G = 1.0f;
        vBoneColorFactor.B = 1.0f;
    }
}

void old_anim_init(z_old_limb *l)
{
    z_old_limb *lTmp;
    int i, j;
    for(i=0;i<l->__align;i++)
    {
        lTmp = old_limb_tree_search(l, i);
        if(lTmp == NULL){
            dbgprintf(0, MSK_COLORTYPE_ERROR, "jjjjjjj");
            continue;
        }
        for(j=0;j<3;j++)
        {
           lTmp->rot[j] = lTmp->rot_d[j];
        }
    }
}

void drawOldBones(u32 BoneOffset, u32 anim, f32 Scale, short X, short Y, short Z, short RX, short RY, short RZ)
{
    z_old_limb *lTop = vCurrentActor.old_limb_top, *lTmp;
    u32 order[256]; // this is big just-in-case. 64 would probably work fine
    u8 abank, *pos, *rindex, *rvals;
    int i=0, j, ind;
    
    RDP_ClearStructures(false);

    if(!RDP_CheckAddressValidity(BoneOffset)){
        return;
    }
    
    if(Scale < 0.001f){
        Scale = 0.02f;
    }
    
    //parse bones
    if(lTop == NULL) /* Lets only do this once */
    {
        pos = RAM[BoneOffset>>24].Data + (BoneOffset & 0xFFFFFF);
        while(i<arraySize(order))
        {
            order[i] = U32(pos);
            if(!order[i])break;
            i++;
            pos+=4;
        }
        vCurrentActor.old_limb_top = lTop = read_z_old_limb(order[0], order);
        /* Hack: this variable is there for 32-bit alignment.
           I store the number of limbs in it */
        lTop->__align = i;
        old_anim_init(lTop);
        vCurrentActor.frameTotal = -1;
    }
    
    //parse animation
    if(RDP_CheckAddressValidity(anim))
    {
        abank = anim>>24;
        anim &= 0xFFFFFF;
        rvals = (u8*)U32(RAM[abank].Data + anim + 4);
        rindex = (u8*)U32(RAM[abank].Data + anim + 8);
        if(!RDP_CheckAddressValidity((u32)rvals) || !RDP_CheckAddressValidity((u32)rindex))
            goto leave;
        if(vCurrentActor.frameTotal != U16(RAM[abank].Data + anim))
        {
            vCurrentActor.frameTotal = U16(RAM[abank].Data + anim);
            old_anim_init(lTop);
        }
        rvals = RAM[abank].Data + (((u32)rvals) & 0xFFFFFF);
        rindex = RAM[abank].Data + (((u32)rindex) & 0xFFFFFF)+12;
        for(i=0;i<lTop->__align;i++)
        {
            lTmp = old_limb_tree_search(lTop, i);
            if(lTmp == NULL){
                dbgprintf(0, MSK_COLORTYPE_ERROR, "jjjjjjj");
                continue;
            }
            for(j=0;j<3;j++)
            {
                ind = U16(rindex);
                rindex += 2;
                if(ind != 1)
                    ind = U16(rindex) * 2 + vCurrentActor.frameCurrent * 2;
                else
                    ind = U16(rindex) * 2;
                lTmp->rot[j] = U16(rvals + ind);
                rindex+=2;
            }
        }
    } else if (vCurrentActor.frameTotal) {
        old_anim_init(lTop);
        vCurrentActor.frameTotal = 0;
    }
   leave:
    
    vBoneColorFactor.R = 0.0f;
    vBoneColorFactor.G = 0.0f;
    vBoneColorFactor.B = 0.0f;

    //render
    glPushMatrix();
        glScalef(Scale, Scale, Scale);
        drawOldBone(lTop);
    glPopMatrix();
}
