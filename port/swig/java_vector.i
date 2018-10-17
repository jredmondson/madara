/**

@file pass_vector_by_value_java.i

These typemaps give more natural support for std::vector, mapping
std::vector<any primitive C++ type> from/to a Java array of the
corresponding primitive type.

The typemaps are not efficient as there is a lot of copying of the
vector values whenever the array is passed to C++ from Java and
viceversa. 

It only works for pass-by-value; no pass-by-reference (not even const
reference), nor pass-by-pointer.

Example usage:

  Wrapping:

     %include "pass_vector_by_value_java.i"
     %inline %{
       extern "C" {
         void foo(std::vector<int> bar);
         std::vector<int> baz(int qux);
       };
     %}

  Use from Java like this:

    example.foo(new int[] {1,2,3});
    int[] xyz = example.baz(5);  

This was copied from arrays_java.i in the SWIG 1.3.24 distribution,
and slightly adapted by Riccardo Murri <riccardo.murri@ictp.it>.

  */


%{
// these typemaps work only with C++
#ifndef __cplusplus
#error "pass_vector_by_value.i typemaps can only be used within C++ code!"
#endif
%}


%{
#include <vector>  // std::vector
%}


// macros to add support for a generic container type, somewhen in the future...
#define VECTOR_JAVA_CONTAINER_DECL(CTYPE)              std::vector<CTYPE>
#define VECTOR_JAVA_CONTAINER_APPEND(CONTAINER,RVALUE) (CONTAINER).push_back(RVALUE)
#define VECTOR_JAVA_CONTAINER_PREALLOC(CONTAINER,SZ)   (CONTAINER).reserve(SZ)


/* Primitive array support is a combination of SWIG macros and functions in order to reduce 
 * code bloat and aid maintainability. The SWIG preprocessor expands the macros into functions 
 * for inclusion in the generated code. */


/* Vector support functions declarations macro */
%define JAVA_VECTOR_DECL(CTYPE, JNITYPE, JAVATYPE, JFUNCNAME)
%{
// prototype for JAVATYPE[] --> std::vector<CTYPE> conversion
int SWIG_JavaVectorIn##JFUNCNAME (JNIEnv *jenv, JNITYPE **jarr, 
                                  std::vector<CTYPE> *output_container, 
                                  JNITYPE##Array input);
// prototypes for std::vector<CTYPE> --> JAVATYPE[] conversion
void SWIG_JavaVectorArgout##JFUNCNAME (JNIEnv *jenv, JNITYPE *jarr, 
                                       std::vector<CTYPE> &input_container,
                                       JNITYPE##Array input);
JNITYPE##Array SWIG_JavaVectorOut##JFUNCNAME (JNIEnv *jenv, 
                                              std::vector<CTYPE> &input_container,
                                              jsize sz);
%}
%enddef


/* Array support functions macro */
%define JAVA_VECTOR_IMPL(CTYPE, JNITYPE, JAVATYPE, JFUNCNAME)
%{
  // JAVATYPE[] (JNITYPE##Array) --> std::vector<CTYPE> 
int SWIG_JavaVectorIn##JFUNCNAME (JNIEnv *jenv, JNITYPE **jarr, 
                                  std::vector<CTYPE> *output_container, 
                                  JNITYPE##Array input) 
{
  if(!input) {
    SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null array");
    return 0;
  }
  jsize sz;
  sz = JCALL1(GetArrayLength, jenv, input);
  *jarr = JCALL2(Get##JAVATYPE##ArrayElements, jenv, input, 0);
  if(!*jarr)
    return 0; 
 try {
   //*output_container =  new std::vector<CTYPE>;
   VECTOR_JAVA_CONTAINER_PREALLOC(*output_container,sz);
 }
 catch (std::bad_alloc err) {
    SWIG_JavaThrowException(jenv, SWIG_JavaOutOfMemoryError, 
                            "std::vector allocation failed");
    return 0;
 }
 for(int i=0; i<sz; i++)
    JAVA_TYPEMAP_CONTAINER_APPEND(CTYPE);
  return 1;
}

  // std::vector<CTYPE> --> JNITYPE[]
void SWIG_JavaVectorArgout##JFUNCNAME (JNIEnv *jenv, JNITYPE *jarr, 
                                       std::vector<CTYPE> &input_container,
                                       JNITYPE##Array input) 
{
  jsize sz = JCALL1(GetArrayLength, jenv, input);
  JNITYPE *jarrptr = jarr; 
  std::vector<CTYPE>::const_iterator elt = input_container.begin();
  while(elt != input_container.end()) {
    if(0 == sz--)
      // bail out if no more space in Java array
      // XXX: should throw exception?
      break;
    *jarrptr++ = (JNITYPE) *elt;
    ++elt;
  }
  JCALL3(Release##JAVATYPE##ArrayElements, jenv, input, jarr, 0);
}

// std::vector<CTYPE> --> JNITYPE[]
JNITYPE##Array SWIG_JavaVectorOut##JFUNCNAME (JNIEnv *jenv, 
                                              std::vector<CTYPE> &input_container, 
                                              jsize sz) 
{
  JNITYPE##Array jresult = JCALL1(New##JAVATYPE##Array, jenv, sz);
  if(!jresult)
    return NULL;
  JNITYPE *arr = JCALL2(Get##JAVATYPE##ArrayElements, jenv, jresult, 0);
  if(!arr)
    return NULL;
  JNITYPE *jarrptr = arr; 
  std::vector<CTYPE>::const_iterator elt = input_container.begin();
  while(elt != input_container.end()) {
    if(0 == sz--)
      // bail out if no more space in Java array
      // XXX: should throw exception?
      break;
    *jarrptr++ = (JNITYPE) *elt;
    ++elt;
  }
  JCALL3(Release##JAVATYPE##ArrayElements, jenv, jresult, arr, 0);
  return jresult;
}
%}
%enddef


%{
#if defined(SWIG_NOINCLUDE) || defined(SWIG_NOARRAYS)
%}

JAVA_VECTOR_DECL(bool, jboolean, Boolean, Bool)       /* bool[] */
JAVA_VECTOR_DECL(signed char, jbyte, Byte, Schar)     /* signed char[] */
JAVA_VECTOR_DECL(unsigned char, jshort, Short, Uchar) /* unsigned char[] */
JAVA_VECTOR_DECL(short, jshort, Short, Short)         /* short[] */
JAVA_VECTOR_DECL(unsigned short, jint, Int, Ushort)   /* unsigned short[] */
JAVA_VECTOR_DECL(int, jint, Int, Int)                 /* int[] */
JAVA_VECTOR_DECL(unsigned int, jlong, Long, Uint)     /* unsigned int[] */
JAVA_VECTOR_DECL(long, jint, Int, Long)               /* long[] */
JAVA_VECTOR_DECL(unsigned long, jlong, Long, Ulong)   /* unsigned long[] */
JAVA_VECTOR_DECL(jlong, jlong, Long, Longlong)        /* long long[] */
JAVA_VECTOR_DECL(float, jfloat, Float, Float)         /* float[] */
JAVA_VECTOR_DECL(double, jdouble, Double, Double)     /* double[] */

%{
#else
%}

// Bool array element assignment different to other types to keep Visual C++ quiet
#define JAVA_TYPEMAP_CONTAINER_APPEND(CTYPE) VECTOR_JAVA_CONTAINER_APPEND(*output_container,0!=(*jarr)[i])
JAVA_VECTOR_IMPL(bool, jboolean, Boolean, Bool)       /* bool[] */
#undef JAVA_TYPEMAP_CONTAINER_APPEND

// definition of other types
#define JAVA_TYPEMAP_CONTAINER_APPEND(CTYPE) VECTOR_JAVA_CONTAINER_APPEND(*output_container,(*jarr)[i])
JAVA_VECTOR_IMPL(signed char, jbyte, Byte, Schar)     /* signed char[] */
JAVA_VECTOR_IMPL(unsigned char, jshort, Short, Uchar) /* unsigned char[] */
JAVA_VECTOR_IMPL(short, jshort, Short, Short)         /* short[] */
JAVA_VECTOR_IMPL(unsigned short, jint, Int, Ushort)   /* unsigned short[] */
JAVA_VECTOR_IMPL(int, jint, Int, Int)                 /* int[] */
JAVA_VECTOR_IMPL(unsigned int, jlong, Long, Uint)     /* unsigned int[] */
JAVA_VECTOR_IMPL(long, jint, Int, Long)               /* long[] */
JAVA_VECTOR_IMPL(unsigned long, jlong, Long, Ulong)   /* unsigned long[] */
JAVA_VECTOR_IMPL(jlong, jlong, Long, Longlong)        /* long long[] */
JAVA_VECTOR_IMPL(float, jfloat, Float, Float)         /* float[] */
JAVA_VECTOR_IMPL(double, jdouble, Double, Double)     /* double[] */

%{
#endif
%}



/* The rest of this file has the array typemaps */

// abbrev for defining a typemap for pass by-value, by-reference, by-pointer
#define __VPR(TYPE) TYPE,TYPE&,TYPE*

// Vector of primitive types use the following macro. 
//   The array typemaps use support functions.
%define JAVA_VECTOR_TYPEMAPS(CTYPE, JTYPE, JNITYPE, JFUNCNAME, JNIDESC)

%typemap(jni) std::vector<CTYPE>             %{JNITYPE##Array%}
%typemap(jtype) std::vector<CTYPE>           %{JTYPE[]%}
%typemap(jstype) std::vector<CTYPE>          %{JTYPE[]%}
%typemap(in) std::vector<CTYPE> (JNITYPE *jarr)
{
  printf("hello");
  if(!SWIG_JavaVectorIn##JFUNCNAME(jenv, &jarr, &$1, $input)) 
    return $null; 
}
%typemap(argout) std::vector<CTYPE>
{
  SWIG_JavaVectorArgout##JFUNCNAME(jenv, jarr$argnum, $1, $input); 
}
%typemap(directorin,descriptor=JNIDESC) std::vector<CTYPE>
{
  $input = SWIG_JavaVectorOut##JFUNCNAME(jenv, $1, $1.size()); 
}
%typemap(out) std::vector<CTYPE> 
{
  $result = SWIG_JavaVectorOut##JFUNCNAME(jenv, $1, $1.size());
}
%typemap(freearg) std::vector<CTYPE>         %{ /*delete $1;*/ %}
%typemap(javain) std::vector<CTYPE>          "$javainput"
%typemap(javaout) std::vector<CTYPE>         { return $jnicall; }
%typemap(javadirectorin) std::vector<CTYPE>  "$jniinput"
%typemap(javadirectorout) std::vector<CTYPE> "$javacall"

%enddef


JAVA_VECTOR_TYPEMAPS(bool, boolean, jboolean, Bool, "[Z")       /* bool[ANY] */
JAVA_VECTOR_TYPEMAPS(signed char, byte, jbyte, Schar, "[B")     /* signed char[ANY] */
JAVA_VECTOR_TYPEMAPS(unsigned char, short, jshort, Uchar, "[S") /* unsigned char[ANY] */
JAVA_VECTOR_TYPEMAPS(short, short, jshort, Short, "[S")         /* short[ANY] */
JAVA_VECTOR_TYPEMAPS(unsigned short, int, jint, Ushort, "[I")   /* unsigned short[ANY] */
JAVA_VECTOR_TYPEMAPS(int, int, jint, Int, "[I")                 /* int[ANY] */
JAVA_VECTOR_TYPEMAPS(unsigned int, long, jlong, Uint, "[J")     /* unsigned int[ANY] */
JAVA_VECTOR_TYPEMAPS(long, int, jint, Long, "[I")               /* long[ANY] */
JAVA_VECTOR_TYPEMAPS(unsigned long, long, jlong, Ulong, "[J")   /* unsigned long[ANY] */
JAVA_VECTOR_TYPEMAPS(long long, long, jlong, Longlong, "[J")    /* long long[ANY] */
JAVA_VECTOR_TYPEMAPS(float, float, jfloat, Float, "[F")         /* float[ANY] */
JAVA_VECTOR_TYPEMAPS(double, double, jdouble, Double, "[D")     /* double[ANY] */
