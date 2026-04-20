/*
 * main.c - Lab: Hello, 3-address Code
 *
 * Builds the TAC linked list for:
 *
 *   fun main() {
 *      var i : Int = 5;
 *      i = i * i + 1;
 *      println("Variable i is $i.");
 *   }
 *
 * Target TAC (from the lab spec):
 *
 *   .string 8
 *       "Variable i is %d.\0"
 *   .code
 *   proc main,0,32
 *       ASN    loc:0,  const:5
 *       MUL    loc:8,  loc:0,   loc:0
 *       ADD    loc:16, loc:8,   const:1
 *       ASN    loc:0,  loc:16
 *       PARAM  loc:0
 *       PARAM  str:0
 *       CALL   printf, const:2, loc:24
 *       RETURN
 *   end main
 */

#include <stdio.h>
#include <stdlib.h>
#include "tac.h"

int main(void)
{
   //start with none
   struct addr NONE = addr_make(R_NONE, 0);

   //variable temp addresses
   struct addr i      = addr_make(R_LOCAL,  0);
   struct addr t1     = addr_make(R_LOCAL,  8);
   struct addr t2     = addr_make(R_LOCAL, 16);
   struct addr t3     = addr_make(R_LOCAL, 24);

   //the constants
   struct addr c5     = addr_make(R_CONST, 5);
   struct addr c1     = addr_make(R_CONST, 1);
   struct addr c2     = addr_make(R_CONST, 2);  

   //string literal offset
   struct addr str0   = addr_make(R_STRING, 0);

   
   struct addr printf_name = addr_name(R_NAME, "printf");

   
   // Print the string section header (assembler pseudo-output).
   
   printf(".string 8\n");
   printf("\t\"Variable i is %%d.\\000\"\n");
   printf(".code\n");
   printf("proc main,0,32\n");

   //build instruction list
   struct instr *code = NULL;

   code = append(code, gen(O_ASN, i, c5, NONE));
   code = append(code, gen(O_MUL, t1, i, i));
   code = append(code, gen(O_ADD, t2, t1, c1));
   code = append(code, gen(O_ASN, i, t2, NONE));
   code = append(code, gen(O_PARM, i, NONE, NONE));
   code = append(code, gen(O_PARM, str0, NONE, NONE));
   code = append(code, gen(O_CALL, printf_name, c2, t3));
   code = append(code, gen(O_RET, NONE, NONE, NONE));
   tacprint(code);

   printf("end o'main\n");

   return 0;
}