/*------------------------------------------------------------------------
 *
 * Derivative of the OpenVG 1.0.1 Reference Implementation
 * -------------------------------------
 *
 * Copyright (c) 2007 The Khronos Group Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and /or associated documentation files
 * (the "Materials "), to deal in the Materials without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Materials,
 * and to permit persons to whom the Materials are furnished to do so,
 * subject to the following conditions: 
 *
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Materials. 
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE MATERIALS OR
 * THE USE OR OTHER DEALINGS IN THE MATERIALS.
 *
 *-------------------------------------------------------------------*/

/*-------------------------------------------------------------------*//*!
* \brief	Applies paint, image drawing, masking and blending at pixel (x,y).
* \param	
* \return	
* \note		premultiplied blending formulas
			//src
			a = asrc
			r = rsrc
			//src over
			a = asrc + adst * (1-asrc)
			r = rsrc + rdst * (1-asrc)
			//dst over
			a = asrc * (1-adst) + adst
			r = rsrc * (1-adst) + adst
			//src in
			a = asrc * adst
			r = rsrc * adst
			//dst in
			a = adst * asrc
			r = rdst * asrc
			//multiply
			a = asrc + adst * (1-asrc)
			r = rsrc * (1-adst) + rdst * (1-asrc) + rsrc * rdst
			//screen
			a = asrc + adst * (1-asrc)
			r = rsrc + rdst - rsrc * rdst
			//darken
			a = asrc + adst * (1-asrc)
			r = MIN(rsrc + rdst * (1-asrc), rdst + rsrc * (1-adst))
			//lighten
			a = asrc + adst * (1-asrc)
			r = MAX(rsrc + rdst * (1-asrc), rdst + rsrc * (1-adst))
			//additive
			a = MIN(asrc+adst,1)
			r = rsrc + rdst
*//*-------------------------------------------------------------------*/




static inline float colorFromTemp(float c,float q,float p){
    if(6.0*c<1)
     c=p+(q-p)*6.0*c;
    else if(2.0*c<1)
     c=q;
    else if(3.0*c<2)
     c=p+(q-p)*((2.0/3.0)-c)*6.0;
    else
     c=p;
    return c;
}

static inline void HSLToRGB(float hue,float saturation,float luminance,float *redp,float *greenp,float *bluep) {
   float red=luminance,green=luminance,blue=luminance;

   if(saturation!=0){
    float p,q;

    if(luminance<0.5)
     q=luminance*(1+saturation);
    else
     q=luminance+saturation-(luminance*saturation);
    p=2*luminance-q;
    
    red=hue+1.0/3.0;
    if(red<0)
     red+=1.0;
    green=hue;
    if(green<0)
     green+=1.0;
    blue=hue-1.0/3.0;
    if(blue<0)
     blue+=1.0;
    
    red=colorFromTemp(red,q,p);
    green=colorFromTemp(green,q,p);
    blue=colorFromTemp(blue,q,p);
   }

   *redp=red;
   *greenp=green;
   *bluep=blue;
}

static inline void RGBToHSL(float r,float g,float b,float *huep,float *saturationp,float *luminancep) {
   float hue=0,saturation=0,luminance,min,max;

   max=MAX(r,MAX(g,b));
   min=MIN(r,MIN(g,b));
   if(max==min)
    hue=0;
   else if(max==r && g>=b)
    hue=60*((g-b)/(max-min));
   else if(max==r && g<b)
    hue=60*((g-b)/(max-min))+360;
   else if(max==g)
    hue=60*((b-r)/(max-min))+120;
   else if(max==b)
    hue=60*((r-g)/(max-min))+240;
    
   luminance=(max+min)/2.0;
   if(max==min)
    saturation=0;
   else if(luminance<=0.5)
    saturation=(max-min)/(max+min);
   else
    saturation=(max-min)/(2-(max+min));
    
   if(huep!=NULL)
    *huep=fmod(hue,360)/360.0;
   if(saturationp!=NULL)
    *saturationp=saturation;
   if(luminancep!=NULL)
    *luminancep=luminance;
}

static void KGBlendSpanNormal_8888(KGRGBA8888 *src,KGRGBA8888 *dst,int length){
// Passes Visual Test
   int i;
   
   for(i=0;i<length;i++){
    KGRGBA8888 s=src[i];
    KGRGBA8888 d=dst[i];
    KGRGBA8888 r;
    unsigned char sa=255-s.a;
    
    r.r=RI_INT_MIN((unsigned)s.r+alphaMultiply(d.r,sa),255);
    r.g=RI_INT_MIN((unsigned)s.g+alphaMultiply(d.g,sa),255);
    r.b=RI_INT_MIN((unsigned)s.b+alphaMultiply(d.b,sa),255);
    r.a=RI_INT_MIN((unsigned)s.a+alphaMultiply(d.a,sa),255);
    
    src[i]=r;
   }
}

static void KGBlendSpanNormal_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// Passes Visual Test
   int i;
   
   for(i=0;i<length;i++){
    KGRGBAffff s=src[i];
    KGRGBAffff d=dst[i];
    KGRGBAffff r;
    
    r.r = s.r + d.r * (1.0f - s.a);
    r.g = s.g + d.g * (1.0f - s.a);
    r.b = s.b + d.b * (1.0f - s.a);
    r.a = s.a + d.a * (1.0f - s.a);
    
    src[i]=r;
   }
}

static void KGBlendSpanMultiply_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// Passes Visual Test
   int i;
   
   for(i=0;i<length;i++){
    KGRGBAffff s=src[i];
    KGRGBAffff d=dst[i];
    KGRGBAffff r;

    r.r = s.r * (1.0f - d.a + d.r) + d.r * (1.0f - s.a);
    r.g = s.g * (1.0f - d.a + d.g) + d.g * (1.0f - s.a);
    r.b = s.b * (1.0f - d.a + d.b) + d.b * (1.0f - s.a);
    r.a = s.a + d.a * (1.0f - s.a);
    
    src[i]=r;
   }
}

static void KGBlendSpanScreen_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// Passes Visual Test
   int i;
   
   for(i=0;i<length;i++){
    KGRGBAffff s=src[i];
    KGRGBAffff d=dst[i];
    KGRGBAffff r;
    
    r.r = s.r + d.r - s.r*d.r;
    r.g = s.g + d.g - s.g*d.g;
    r.b = s.b + d.b - s.b*d.b;
    r.a = s.a + d.a * (1.0f - s.a);

    src[i]=r;
   }
}

static void KGBlendSpanOverlay_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// broken
   int i;
   
   for(i=0;i<length;i++){
    KGRGBAffff s=src[i];
    KGRGBAffff d=dst[i];
    KGRGBAffff r;
    
    CGFloat max=RI_MAX(s.r,RI_MAX(s.g,s.b));
    CGFloat min=RI_MIN(s.r,RI_MIN(s.g,s.b));
    CGFloat lum=(max+min)/2*(1.0-d.a);
    if(lum<=0.5)
     r.r = s.r * (1.0f - d.a + d.r) + d.r * (1.0f - s.a);
    else
     r.r = s.r + d.r - s.r*d.r;

    if(lum<=0.5)
     r.g = s.g * (1.0f - d.a + d.g) + d.g * (1.0f - s.a);
    else
     r.g = s.g + d.g - s.g*d.g;
        
    if(lum<=0.5)
     r.b = s.b * (1.0f - d.a + d.b) + d.b * (1.0f - s.a);
    else
     r.b = s.b + d.b - s.b*d.b;

    r.a = s.a + d.a * (1.0f - s.a);

    src[i]=r;
   }
}
static void KGBlendSpanDarken_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// Passes Visual Test
   int i;
   
   for(i=0;i<length;i++){
    KGRGBAffff s=src[i];
    KGRGBAffff d=dst[i];
    KGRGBAffff r;
    
    r.r = RI_MIN(s.r + d.r * (1.0f - s.a), d.r + s.r * (1.0f - d.a));
    r.g = RI_MIN(s.g + d.g * (1.0f - s.a), d.g + s.g * (1.0f - d.a));
    r.b = RI_MIN(s.b + d.b * (1.0f - s.a), d.b + s.b * (1.0f - d.a));
    r.a = s.a + d.a * (1.0f - s.a);
    src[i]=r;
   }
}
static void KGBlendSpanLighten_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// Passes Visual Test
   int i;
   
   for(i=0;i<length;i++){
    KGRGBAffff s=src[i];
    KGRGBAffff d=dst[i];
    KGRGBAffff r;
    
    r.r = RI_MAX(s.r + d.r * (1.0f - s.a), d.r + s.r * (1.0f - d.a));
    r.g = RI_MAX(s.g + d.g * (1.0f - s.a), d.g + s.g * (1.0f - d.a));
    r.b = RI_MAX(s.b + d.b * (1.0f - s.a), d.b + s.b * (1.0f - d.a));
    //although the statement below is equivalent to r.a = s.a + d.a * (1.0f - s.a)
    //in practice there can be a very slight difference because
    //of the max operation in the blending formula that may cause color to exceed alpha.
    //Because of this, we compute the result both ways and return the maximum.
    r.a = RI_MAX(s.a + d.a * (1.0f - s.a), d.a + s.a * (1.0f - d.a));
    src[i]=r;
   }
}
static void KGBlendSpanColorDodge_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// broken
   int i;
   
   for(i=0;i<length;i++){
    KGRGBAffff s=src[i];
    KGRGBAffff d=dst[i];
    KGRGBAffff r;

    r.r=(s.r==1.0f)?1.0f:RI_MIN(1.0f,d.r/(1.0f-s.r));
    r.g=(s.g==1.0f)?1.0f:RI_MIN(1.0f,d.g/(1.0f-s.g));
    r.b=(s.b==1.0f)?1.0f:RI_MIN(1.0f,d.b/(1.0f-s.b));

    r.a = s.a + d.a * (1.0f - s.a);
    src[i]=r;
   }
}
static void KGBlendSpanColorBurn_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// broken
   int i;
   
   for(i=0;i<length;i++){
    KGRGBAffff s=src[i];
    KGRGBAffff d=dst[i];
    KGRGBAffff r;

    r.r=(s.r==0)?0:1.0-RI_MIN(1.0,(1.0-d.r)/s.r);
    r.g=(s.g==0)?0:1.0-RI_MIN(1.0,(1.0-d.g)/s.g);
    r.b=(s.b==0)?0:1.0-RI_MIN(1.0,(1.0-d.b)/s.b);
    r.a = s.a + d.a * (1.0f - s.a);
    src[i]=r;
   }
}
static void KGBlendSpanHardLight_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// broken
   int i;
   
   for(i=0;i<length;i++){
    KGRGBAffff s=src[i];
    KGRGBAffff d=dst[i];
    KGRGBAffff r=d;
    

    if(s.r<=0.5){
     s.r*=2;
     r.r = s.r * (1.0f - d.a + d.r) + d.r * (1.0f - s.a);
    }
    else {
     s.r=2*s.r-1;
     r.r = s.r + d.r - s.r*d.r;
    }
    
    if(s.g<=0.5){
     s.g*=2;
     r.g = s.g * (1.0f - d.a + d.g) + d.g * (1.0f - s.a);
    }
    else {
     s.g=2*s.g-1;
     r.g = s.g + d.g - s.g*d.g;
    }
    
    if(s.b<=0.5){
     s.b*=2;
     r.b = s.b * (1.0f - d.a + d.b) + d.b * (1.0f - s.a);
    }
    else {
     s.b=2*s.b-1;
     r.b = s.b + d.b - s.b*d.b;
    }
    
    r.a = s.a + d.a * (1.0f - s.a);
    src[i]=r;
   }
}
static void KGBlendSpanSoftLight_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// broken
   int i;
   
   for(i=0;i<length;i++){
    //KGRGBAffff s=src[i];
    KGRGBAffff d=dst[i];
    KGRGBAffff r=d;
    
    src[i]=r;
   }
}
static void KGBlendSpanDifference_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// Passes Visual Test
   int i;
   
   for(i=0;i<length;i++){
    KGRGBAffff s=src[i];
    KGRGBAffff d=dst[i];
    KGRGBAffff r;
    
    r.r=s.r+d.r-2*(RI_MIN(s.r*d.a,d.r*s.a));
    r.g=s.g+d.g-2*(RI_MIN(s.g*d.a,d.g*s.a));
    r.b=s.b+d.b-2*(RI_MIN(s.b*d.a,d.b*s.a));
    r.a = s.a + d.a * (1.0f - s.a);
    src[i]=r;
   }
}
static void KGBlendSpanExclusion_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// Passes Visual Test
   int i;
   
   for(i=0;i<length;i++){
    KGRGBAffff s=src[i];
    KGRGBAffff d=dst[i];
    KGRGBAffff r;
    
    r.r = (s.r * d.a + d.r * s.a - 2 * s.r * d.r) + s.r * (1 - d.a) + d.r * (1 - s.a);
    r.g = (s.g * d.a + d.g * s.a - 2 * s.g * d.g) + s.g * (1 - d.a) + d.g * (1 - s.a);
    r.b = (s.b * d.a + d.b * s.a - 2 * s.b * d.b) + s.b * (1 - d.a) + d.b * (1 - s.a);
    r.a = s.a + d.a * (1.0f - s.a);
    src[i]=r;
   }
}
static void KGBlendSpanHue_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// broken
   int i;
   
   for(i=0;i<length;i++){
    KGRGBAffff s=src[i];
    KGRGBAffff d=dst[i];
    KGRGBAffff r;
    
    CGFloat sh,ss,sl;
    CGFloat dh,ds,dl;
        
    RGBToHSL(s.r,s.g,s.b,&sh,&ss,&sl);
    RGBToHSL(d.r,d.g,d.b,&dh,&ds,&dl);
    HSLToRGB(sh,ds,dl,&r.r,&r.g,&r.b);
    r.a = s.a + d.a * (1.0f - s.a);
    src[i]=r;
   }
}
static void KGBlendSpanSaturation_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// broken
   int i;
   
   for(i=0;i<length;i++){
    KGRGBAffff s=src[i];
    KGRGBAffff d=dst[i];
    KGRGBAffff r;
    
    CGFloat sh,ss,sl;
    CGFloat dh,ds,dl;
        
    RGBToHSL(s.r,s.g,s.b,&sh,&ss,&sl);
    RGBToHSL(d.r,d.g,d.b,&dh,&ds,&dl);
    HSLToRGB(dh,ss,dl,&r.r,&r.g,&r.b);
    r.a = s.a + d.a * (1.0f - s.a);
    src[i]=r;
   }
}
static void KGBlendSpanColor_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// broken
   int i;
   
   for(i=0;i<length;i++){
    //KGRGBAffff s=src[i];
    KGRGBAffff d=dst[i];
    KGRGBAffff r=d;
    
    src[i]=r;
   }
}
static void KGBlendSpanLuminosity_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// broken
   int i;
   
   for(i=0;i<length;i++){
    //KGRGBAffff s=src[i];
    KGRGBAffff d=dst[i];
    KGRGBAffff r=d;
    
    src[i]=r;
   }
}

static void KGBlendSpanClear_8888(KGRGBA8888 *src,KGRGBA8888 *dst,int length){
// Passes Visual Test
   int i;
   
   for(i=0;i<length;i++){
    KGRGBA8888 r;
    
    r.r=0;
    r.g=0;
    r.b=0;
    r.a=0;

    src[i]=r;
   }
}

static void KGBlendSpanClear_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// Passes Visual Test
   int i;
   
   for(i=0;i<length;i++){
    KGRGBAffff r;
    
    r.r=0;
    r.g=0;
    r.b=0;
    r.a=0;

    src[i]=r;
   }
}

static void KGBlendSpanCopy_8888(KGRGBA8888 *src,KGRGBA8888 *dst,int length){
// Passes Visual Test
   // do nothing src already contains values
}

static void KGBlendSpanCopy_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// Passes Visual Test
   // do nothing src already contains values
}

static void KGBlendSpanSourceIn_8888(KGRGBA8888 *src,KGRGBA8888 *dst,int length){
// Passes Visual Test
   int i;
   
   for(i=0;i<length;i++){
    KGRGBA8888 s=src[i];
    KGRGBA8888 d=dst[i];
    KGRGBA8888 r;

    r.r = alphaMultiply(s.r , d.a);
    r.g = alphaMultiply(s.g , d.a);
    r.b = alphaMultiply(s.b , d.a);
    r.a = alphaMultiply(s.a , d.a);

    src[i]=r;
   }
}

static void KGBlendSpanSourceIn_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// Passes Visual Test
   int i;
   
   for(i=0;i<length;i++){
    KGRGBAffff s=src[i];
    KGRGBAffff d=dst[i];
    KGRGBAffff r;
    
    r.r = s.r * d.a;
    r.g = s.g * d.a;
    r.b = s.b * d.a;
    r.a = s.a * d.a;
    src[i]=r;
   }
}
static void KGBlendSpanSourceOut_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// Passes Visual Test
   int i;
   
   for(i=0;i<length;i++){
    KGRGBAffff s=src[i];
    KGRGBAffff d=dst[i];
    KGRGBAffff r;
    
    r.r = s.r *(1.0- d.a);
    r.g = s.g * (1.0- d.a);
    r.b = s.b * (1.0- d.a);
    r.a = s.a * (1.0- d.a);
    src[i]=r;
   }
}
static void KGBlendSpanSourceAtop_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// Passes Visual Test
   int i;
   
   for(i=0;i<length;i++){
    KGRGBAffff s=src[i];
    KGRGBAffff d=dst[i];
    KGRGBAffff r;
    
    r.r = s.r*d.a+d.r*(1.0-s.a);
    r.g = s.g*d.a+d.g*(1.0-s.a);
    r.b = s.b*d.a+d.b*(1.0-s.a);
    r.a = s.a*d.a+d.a*(1.0-s.a);
    src[i]=r;
   }
}
static void KGBlendSpanDestinationOver_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// Passes Visual Test
   int i;
   
   for(i=0;i<length;i++){
    KGRGBAffff s=src[i];
    KGRGBAffff d=dst[i];
    KGRGBAffff r;
    
    r.r = s.r * (1.0f - d.a) + d.r;
    r.g = s.g * (1.0f - d.a) + d.g;
    r.b = s.b * (1.0f - d.a) + d.b;
    r.a = s.a * (1.0f - d.a) + d.a;

    src[i]=r;
   }
}
static void KGBlendSpanDestinationIn_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// Passes Visual Test
   int i;
   
   for(i=0;i<length;i++){
    KGRGBAffff s=src[i];
    KGRGBAffff d=dst[i];
    KGRGBAffff r;
    
    r.r = d.r * s.a;
    r.g = d.g * s.a;
    r.b = d.b * s.a;
    r.a = d.a * s.a;
    
    src[i]=r;
   }
}
static void KGBlendSpanDestinationOut_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// Passes Visual Test
   int i;
   
   for(i=0;i<length;i++){
    KGRGBAffff s=src[i];
    KGRGBAffff d=dst[i];
    KGRGBAffff r;
    
    r.r = d.r *(1.0- s.a);
    r.g = d.g * (1.0- s.a);
    r.b = d.b * (1.0- s.a);
    r.a = d.a * (1.0- s.a);

    src[i]=r;
   }
}

static void KGBlendSpanDestinationAtop_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// Passes Visual Test
   int i;
   
   for(i=0;i<length;i++){
    KGRGBAffff s=src[i];
    KGRGBAffff d=dst[i];
    KGRGBAffff r;
    
    r.r=s.r*(1.0-d.a)+d.r*s.a;
    r.g=s.g*(1.0-d.a)+d.g*s.a;
    r.b=s.b*(1.0-d.a)+d.b*s.a;
    r.a=s.a*(1.0-d.a)+d.a*s.a;
    
    src[i]=r;
   }
}

static void KGBlendSpanXOR_8888(KGRGBA8888 *src,KGRGBA8888 *dst,int length){
// Passes Visual Test
   int i;
   
   for(i=0;i<length;i++){
    KGRGBA8888 s=src[i];
    KGRGBA8888 d=dst[i];
    KGRGBA8888 r;
    
    r.r=RI_INT_MIN(((unsigned)s.r*(255-(unsigned)d.a)+(unsigned)d.r*(255-(unsigned)s.a))/255,255);
    r.g=RI_INT_MIN(((unsigned)s.g*(255-(unsigned)d.a)+(unsigned)d.g*(255-(unsigned)s.a))/255,255);
    r.b=RI_INT_MIN(((unsigned)s.b*(255-(unsigned)d.a)+(unsigned)d.b*(255-(unsigned)s.a))/255,255);
    r.a=RI_INT_MIN(((unsigned)s.a*(255-(unsigned)d.a)+(unsigned)d.a*(255-(unsigned)s.a))/255,255);
    
    src[i]=r;
   }
}

static void KGBlendSpanXOR_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// Passes Visual Test
   int i;
   
   for(i=0;i<length;i++){
    KGRGBAffff s=src[i];
    KGRGBAffff d=dst[i];
    KGRGBAffff r;
    
    r.r=s.r*(1.0-d.a)+d.r*(1.0-s.a);
    r.g=s.g*(1.0-d.a)+d.g*(1.0-s.a);
    r.b=s.b*(1.0-d.a)+d.b*(1.0-s.a);
    r.a=s.a*(1.0-d.a)+d.a*(1.0-s.a);
    
    src[i]=r;
   }
}

static void KGBlendSpanPlusDarker_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// broken
   int i;
   
   for(i=0;i<length;i++){
    KGRGBAffff s=src[i];
    KGRGBAffff d=dst[i];
    KGRGBAffff r;
    
#if 1
// Doc.s say:  R = MAX(0, (1 - D) + (1 - S)). No workie.
    r.r = RI_MAX(0,(1-d.r)+(1-s.r));
    r.g = RI_MAX(0,(1-d.g)+(1-s.g));
    r.b = RI_MAX(0,(1-d.b)+(1-s.b));
    r.a = RI_MAX(0,(1-d.a)+(1-s.a));
#endif
    src[i]=r;
   }
}

static void KGBlendSpanPlusLighter_8888(KGRGBA8888 *src,KGRGBA8888 *dst,int length){
// Passes Visual Test
// Doc.s say: R = MIN(1, S + D). That works
   int i;
   
   for(i=0;i<length;i++){
    KGRGBA8888 s=src[i];
    KGRGBA8888 d=dst[i];
    KGRGBA8888 r;
    
    r.r = RI_INT_MIN((unsigned)s.r + (unsigned)d.r, 255);
    r.g = RI_INT_MIN((unsigned)s.g + (unsigned)d.g, 255);
    r.b = RI_INT_MIN((unsigned)s.b + (unsigned)d.b, 255);
    r.a = RI_INT_MIN((unsigned)s.a + (unsigned)d.a, 255);

    src[i]=r;
   }
}

static void KGBlendSpanPlusLighter_ffff(KGRGBAffff *src,KGRGBAffff *dst,int length){
// Passes Visual Test
// Doc.s say: R = MIN(1, S + D). That works
   int i;
   
   for(i=0;i<length;i++){
    KGRGBAffff s=src[i];
    KGRGBAffff d=dst[i];
    KGRGBAffff r;
    
    r.r = RI_MIN(s.r + d.r, 1.0f);
    r.g = RI_MIN(s.g + d.g, 1.0f);
    r.b = RI_MIN(s.b + d.b, 1.0f);
    r.a = RI_MIN(s.a + d.a, 1.0f);

    src[i]=r;
   }
}
