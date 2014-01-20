%module matrix
%{
/* Includes the header in the wrapper code */
#include "gfx.hpp"
#include "pixel.hpp"
#include "matrix.hpp"
%}

#define __attribute__(x)
/* Parse the header file to generate wrappers */

%include "gfx.hpp"
%include "pixel.hpp"
%include "matrix.hpp"