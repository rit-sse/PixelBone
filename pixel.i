 %module pixel
 %{
 /* Includes the header in the wrapper code */
 #include "pixel.hpp"
 %}
 
 /* Parse the header file to generate wrappers */
 %include "pixel.hpp"
 