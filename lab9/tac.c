/*
 * Three Address Code - skeleton for CS 423
 */
#include <stdio.h>
#include <stdlib.h>
#include "tac.h"

char *regionnames[] = {"global","loc", "class", "lab", "const", "", "none", "str"};
char *regionname(int i) { return regionnames[i-R_GLOBAL]; }
char *opcodenames[] = {
   "ADD","SUB", "MUL", "DIV", "NEG", "ASN", "ADDR", "LCONT", "SCONT", "GOTO",
   "BLT", "BLE", "BGT", "BGE", "BEQ", "BNE", "BIF", "BNIF", "PARM", "CALL",
   "RETURN"
   };
char *opcodename(int i) { return opcodenames[i-O_ADD]; }
char *pseudonames[] = {
   "glob","proc", "loc", "lab", "end", "prot"
   };
char *pseudoname(int i) { return pseudonames[i-D_GLOB]; }

int labelcounter;

//our new helper additions
struct addr addr_make(int region, int offset)
{
   struct addr a;
   a.region = region;
   a.u.offset = offset;
   return a;
}
 
struct addr addr_name(int region, char *name)
{
   struct addr a;
   a.region = region;
   a.u.name = name;
   return a;
}

struct addr *genlabel()
{
   struct addr *a = malloc(sizeof(struct addr));
   a->region = R_LABEL;
   a->u.offset = labelcounter++;
  // printf("generated a label %d\n", a->u.offset); took out for cleaner output
   return a;
}

struct instr *gen(int op, struct addr a1, struct addr a2, struct addr a3)
{
  struct instr *rv = malloc(sizeof (struct instr));
  if (rv == NULL) {
     fprintf(stderr, "out of memory\n");
     exit(4);
     }
  rv->opcode = op;
  rv->dest = a1;
  rv->src1 = a2;
  rv->src2 = a3;
  rv->next = NULL;
  return rv;
}

struct instr *copylist(struct instr *l)
{
   if (l == NULL) return NULL;
   struct instr *lcopy = gen(l->opcode, l->dest, l->src1, l->src2);
   lcopy->next = copylist(l->next);
   return lcopy;
}

struct instr *append(struct instr *l1, struct instr *l2)
{
   if (l1 == NULL) return l2;
   struct instr *ltmp = l1;
   while(ltmp->next != NULL) ltmp = ltmp->next;
   ltmp->next = l2;
   return l1;
}

struct instr *concat(struct instr *l1, struct instr *l2)
{
   return append(copylist(l1), l2);
}

//another lil helper, gives a single address operand
static void print_addr(struct addr a)
{
   if (a.region == R_NONE)
      return;
   if (a.region == R_NAME || a.region == R_GLOBAL) {
      printf("%s", a.u.name);
      return;
   }
   printf("%s:%d", regionname(a.region), a.u.offset);
}

//our tacprint function
void tacprint(struct instr *l)
{
   for (; l != NULL; l = l->next) {
      int op = l->opcode;
 
      //pseudo/declaration instructions
      if (op >= D_GLOB && op <= D_PROT) {
         printf("\t.%s\t", pseudoname(op));
         print_addr(l->dest);
         if (l->src1.region != R_NONE) { printf(","); print_addr(l->src1); }
         if (l->src2.region != R_NONE) { printf(","); print_addr(l->src2); }
         printf("\n");
         continue;
      }
 
      //RETURN: no operands 
      if (op == O_RET) {
         printf("\tRETURN\n");
         continue;
      }
 
      //PARAM: one operand (stored in dest) 
      if (op == O_PARM) {
         printf("\tPARAM\t");
         print_addr(l->dest);
         printf("\n");
         continue;
      }
 
      //CALL: dest=name, src1=nargs, src2=return-value target 
      if (op == O_CALL) {
         printf("\tCALL\t");
         print_addr(l->dest);            //function name 
         printf(",");
         print_addr(l->src1);            //number of args 
         if (l->src2.region != R_NONE) {
            printf(",");
            print_addr(l->src2);         //return-value target 
         }
         printf("\n");
         continue;
      }
 
      // branch: single label operand in dest 
      if (op == O_GOTO || op == O_BIF || op == O_BNIF) {
         printf("\t%s\t", opcodename(op));
         print_addr(l->dest);
         if (l->src1.region != R_NONE) { printf(","); print_addr(l->src1); }
         printf("\n");
         continue;
      }
 
      // other opcodes
      printf("\t%s\t", opcodename(op));
      print_addr(l->dest);
      if (l->src1.region != R_NONE) { printf(","); print_addr(l->src1); }
      if (l->src2.region != R_NONE) { printf(","); print_addr(l->src2); }
      printf("\n");
   }
}
