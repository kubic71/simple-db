#include "pc_main.h"
#include <stdio.h>
#include <string.h>
#include "SQL_parser.h"
#include "table.h"

static void print_usage()
{
   printf("Usage:\n");
   printf("To execute protocol engine:\n");
   printf("$ ./simple-db pc\n\n");

   printf("To start transaction manager:\n");
   printf("$ ./simple-db tm\n");
}

int main(int argc, char **argv)
{
   if (argc != 2)
   {
      print_usage();
      return 1;
   }

   char *service = argv[1];

   if (strcmp(service, "pc") == 0)
   {
      // start protocol-engine
      protocol_engine();
   }
   else if (strcmp(service, "tm") == 0)
   {
      // start transaction-manager
      printf("Transaction manager not implemented yet!");
   }
   else
   {
      print_usage();
      return 1;
   }
}
