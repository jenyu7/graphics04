#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"

//FUNCTION FROM SHELL PROJECT IN SYSTEMS
//added conversion to double functionality

/*======== char ** parse_args() ==========
  Inputs: char * line, char * delim
  Returns: int ** [array of strings converted to doubles]
  Starts with parsing five arguments using strsep
  If more than five args, array dynamically resized with +5 more arguments
  =========================================*/
double * parse_args( char * line, char * delim){
  int size = 6; // start with 5 args
  double * args = malloc( size * sizeof(double));
  int n = 0;
  while( line ){
    args[n] = atof(strsep( &line, delim));
    //dynamic sizing
    if ( ++n == size) {
      size += 5; // add 5 args as necessary
      args = realloc( args, size * sizeof(double));
    }
  }
  args[n] = 0;
  return args;
}


/*======== void parse_file () ==========
Inputs:   char * filename 
          struct matrix * transform, 
          struct matrix * pm,
          screen s
Returns:  3void type

Goes through the file named filename and performs all of the actions listed in that file.
The file follows the following format:
     Every command is a single character that takes up a line
     Any command that requires arguments must have those arguments in the second line.
See the file script for an example of the file format
====================*/
void parse_file ( char * filename, 
                  struct matrix * transform, 
                  struct matrix * edges,
                  screen s) {

  FILE *f;
  char line[256];
  double * args;
  struct matrix * tmp;
  clear_screen(s);
  color c;
  c.red = 80;
  c.green = 100;
  c.blue = 120;

  if ( strcmp(filename, "stdin") == 0 ) 
    f = stdin;
  else
    f = fopen(filename, "r");
  
  while ( fgets(line, 255, f) != NULL ) {
    line[strlen(line)-1]='\0';
    printf(":%s:\n",line);
    //START determining what action to take
    /*
      line: add a line to the edge matrix - 
      takes 6 arguments (x0, y0, z0, x1, y1, z1)
     */
    if(!strcmp(line, "line")){
      fgets(line, 256, f);
      line[strlen(line)-1] = '\0';
      args = parse_args(line, " ");
      add_edge(edges, args[0], args[1], args[2], args[3], args[4], args[5]);
      free(args);
    }
    //ident: set the transform matrix to the identity matrix
    else if(!strcmp(line, "ident")){
      ident(transform);
    }
    /*
      scale: create a scale matrix, 
      then multiply the transform matrix by the scale matrix - 
      takes 3 arguments (sx, sy, sz)
    */
    else if(!strcmp(line, "scale")){
      fgets(line, 255, f);
      line[strlen(line)-1] = '\0';
      args = parse_args(line, " ");
      tmp = make_scale(args[0], args[1], args[2]);
      matrix_mult(tmp, transform);
      free(args);
      free(tmp);
    }
    /*
      translate: create a translation matrix, 
      then multiply the transform matrix by the translation matrix - 
      takes 3 arguments (tx, ty, tz)
    */
    else if(!strcmp(line, "move")){
      fgets(line, 255, f);
      line[strlen(line)-1] = '\0';
      args = parse_args(line, " ");
      tmp = make_translate(args[0], args[1], args[2]);
      matrix_mult(tmp, transform);
      free(args);
      free(tmp);
    }
    /*
      rotate: create an rotation matrix,
      then multiply the transform matrix by the rotation matrix -
      takes 2 arguments (axis, theta) axis should be x y or z
    */
    else if(!strcmp(line, "rotate")){
      fgets(line, 255, f);
      line[strlen(line)-1] = '\0';
      printf("rotate line: %s\n", line);
      char * cpy = line;
      char * axis = strsep(&cpy, " ");
      if(!strcmp(axis, "x")){
	tmp = make_rotX(atof(cpy));
      }
      else if(!strcmp(axis, "y")){
	tmp = make_rotY(atof(cpy));
      }
      else if(!strcmp(axis, "z")){
	tmp = make_rotZ(atof(cpy));
      }
      else{
	printf("Invalid axis inputted.\n");
      }
      matrix_mult(tmp, transform);
      free(tmp);
    }
    // apply: apply the current transformation matrix to the edge matrix
    else if(!strcmp(line, "save")){
      fgets(line, 255, f);
      line[strlen(line)-1] = '\0';
      save_extension(s, line);
    }
    /*
      display: draw the lines of the edge matrix to the screen
      display the screen
    */
    else if(!strcmp(line, "apply")){
      matrix_mult(transform, edges);
      ident(transform);
    }
    /*
      save: draw the lines of the edge matrix to the screen
      save the screen to a file -
      takes 1 argument (file name)
    */
    else if(!strcmp(line, "display")){
      clear_screen(s);
      draw_lines(edges, s, c);
      display(s);
    }
    //quit: end parsing
    else if(!strcmp(line, "quit")){
      exit(0);
    }
    else{
      printf("Invalid command inputted.\n");
    }
  }
}
  
