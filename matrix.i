%module matrix
%{
/* Includes the header in the wrapper code */
#include "matrix.hpp"
%}

#define __attribute__(x)
/* Parse the header file to generate wrappers */
%include "stdint.i"
%include "gfx.hpp"
%include "pixel.hpp"
%include "matrix.hpp"
