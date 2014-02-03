%module pixel 
%{
/* Includes the header in the wrapper code */
#include "pixel.hpp"
%}

#define __attribute__(x)

/* Parse the header file to generate wrappers */
%include "pixel.hpp"
 