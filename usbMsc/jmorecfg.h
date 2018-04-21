// jmorecfg.h
#define BITS_IN_JSAMPLE 8
#define MAX_COMPONENTS  3

typedef unsigned char JSAMPLE;
#define GETJSAMPLE(value)  ((int) (value))

#define MAXJSAMPLE  255
#define CENTERJSAMPLE 128

typedef short JCOEF;
typedef unsigned char JOCTET;
#define GETJOCTET(value)  (value)

typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef short INT16;
typedef long INT32;

typedef unsigned int JDIMENSION;
#define JPEG_MAX_DIMENSION  65500L  /* a tad under 64K to prevent overflows */

#define METHODDEF(type)  static type
#define LOCAL(type)      static type
#define GLOBAL(type)     type
#define EXTERN(type)     extern type
#define JMETHOD(type,methodname,arglist)  type (*methodname) arglist
#define FAR

typedef int boolean;
#define FALSE 0   
#define TRUE  1

#ifdef JPEG_INTERNALS
  #define JPEG_INTERNAL_OPTIONS
#endif

#ifdef JPEG_INTERNAL_OPTIONS
  /* Capability options common to encoder and decoder: */
  #define DCT_ISLOW_SUPPORTED /* slow but accurate integer algorithm */
  #define DCT_IFAST_SUPPORTED /* faster, less accurate integer method */
  #define DCT_FLOAT_SUPPORTED /* floating-point: accurate, fast on fast HW */

  /* Encoder capability options: */
  #define C_ARITH_CODING_SUPPORTED    /* Arithmetic coding back end? */
  #undef  C_MULTISCAN_FILES_SUPPORTED /* Multiple-scan JPEG files? */
  #undef  C_PROGRESSIVE_SUPPORTED     /* Progressive JPEG? (Requires MULTISCAN)*/
  #undef  DCT_SCALING_SUPPORTED     /* Input rescaling via DCT? (Requires DCT_ISLOW)*/
  #undef  ENTROPY_OPT_SUPPORTED     /* Optimization of entropy coding parms? */

  /* Note: if you selected 12-bit data precision, it is dangerous to turn off
   * ENTROPY_OPT_SUPPORTED.  The standard Huffman tables are only good for 8-bit
   * precision, so jchuff.c normally uses entropy optimization to compute
   * usable tables for higher precision.  If you don't want to do optimization,
   * you'll have to supply different default Huffman tables.
   * The exact same statements apply for progressive JPEG: the default tables
   * don't work for progressive mode.  (This may get fixed, however.)
   */
  #define INPUT_SMOOTHING_SUPPORTED   /* Input image smoothing option? */

  /* Decoder capability options: */
  #define D_ARITH_CODING_SUPPORTED    /* Arithmetic coding back end? */
  #undef D_MULTISCAN_FILES_SUPPORTED  /* Multiple-scan JPEG files? */
  #undef D_PROGRESSIVE_SUPPORTED      /* Progressive JPEG? (Requires MULTISCAN)*/
  #define IDCT_SCALING_SUPPORTED      /* Output rescaling via IDCT? */
  #undef SAVE_MARKERS_SUPPORTED       /* jpeg_save_markers() needed? */
  #undef BLOCK_SMOOTHING_SUPPORTED    /* Block smoothing? (Progressive only) */
  #undef UPSAMPLE_SCALING_SUPPORTED   /* Output rescaling at upsample stage? */
  #define UPSAMPLE_MERGING_SUPPORTED  /* Fast path for sloppy upsampling? */
  #define QUANT_1PASS_SUPPORTED       /* 1-pass color quantization? */
  #define QUANT_2PASS_SUPPORTED       /* 2-pass color quantization? */

  /* more capability options later, no doubt */
  #define RGB_RED   2 /* Offset of Red in an RGB scanline element */
  #define RGB_GREEN 1 /* Offset of Green */
  #define RGB_BLUE  0 /* Offset of Blue */
  #define RGB_PIXELSIZE 3 /* JSAMPLEs per RGB scanline element */

  #define INLINE         inline     /*!< inline keyword for GNU Compiler       */

  #ifndef MULTIPLIER
  #define MULTIPLIER  int   /* type for fastest integer multiply */
  #endif

  #ifndef FAST_FLOAT
    #ifdef HAVE_PROTOTYPES
      #define FAST_FLOAT  float
    #else
      #define FAST_FLOAT  double
    #endif
  #endif

#endif /* JPEG_INTERNAL_OPTIONS */
