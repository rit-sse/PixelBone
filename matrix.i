 %module matrix
 %{
 /* Includes the header in the wrapper code */
 #include "matrix.hpp"
 %}
 
 /* Parse the header file to generate wrappers */
 %include "matrix.hpp"
 