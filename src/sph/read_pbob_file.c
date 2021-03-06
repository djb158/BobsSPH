/*---------------------------------------------------------------------------*\
|                                                                             |
|  File: read_pbob_file.c                                                     |
|                                                                             |
|  Function: int main(int argc, char *argv[])                                 |
|                                                                             |
|  Purpose:  test code to read in header + particle information from a .pbob  |
|            file                                                             |
|                                                                             |
|  Call:                                                                      |
|            read_pbob_file time_slice=<int> start_index=<int>                |
|                           N=<int> file_name=<FILE NAME>                     |
|                                                                             |
|  Input:   int time_slice      the timeslice to retrieve                     |
|           int start_index     start index of particle to begin retrieval    |
|           int N               number of particles to retrieve               |
|           char *file_name     absolute path .pbob file to read in           |
|                                                                             |
|  Output:  int exit_statuss    either EXIT_SUCCESS or EXIT_FAILURE           |
|                                                                             |
|  Global variables:  none                                                    |
|                                                                             |
|  Required resources:                                                        |
|                     ReadParticle()                                          |
|                     ReadPBOB()                                              |
|                     ReadNodeDescrip()                                       |
|                     CopyAfterEqual()                                        |
|                     ContainsString()                                        |
|                                                                             |
| Error:  return EXIT_FAILURE on error                                        |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
| David Brown 29-Jul-2015   initial version                                   | 
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Copyright (c) 2015,2016 David Brown                                       |
|                                                                             |
|   This file is part of BOBsSPH                                              |
|                                                                             |
|   BOBsSPH is free software: you can redistribute it and/or modify           |
|   it under the terms of the GNU General Public License as published by      |
|   the Free Software Foundation, either version 3 of the License, or         |
|   (at your option) any later version.                                       |
|                                                                             |
|   BOBsSPH is distributed in the hope that it will be useful,                |
|   but WITHOUT ANY WARRANTY; without even the implied warranty of            |
|   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             |
|   GNU General Public License for more details.                              |
|                                                                             |
|   You should have received a copy of the GNU General Public License         |
|   along with BOBsSPH.  If not, see <http://www.gnu.org/licenses/>.          |
|                                                                             |
\*---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>     /* offsetof */
#include "sph_types.h"
#include "pbob.h"
#include "node_descrip.h"
#include "particle.h"

typedef enum {FALSE,TRUE} logical;
#define MAX_CHARS (1024)

extern PARTICLE *ReadParticle(char *file_name,int time_slice,int start_indx,int N);
extern PBOB *ReadPBOB(char *file_name);
extern NODE_DESCRIP *ReadNodeDescrip(char *file_name,int cluster_size);
extern char *CopyAfterEqual(char *target,  char *source);
extern logical ContainsString(char *string1,char *string2);

int main(int argc, char *argv[])
{
  PARTICLE *particle                   = NULL;
  NODE_DESCRIP *node_descrip           = NULL;
  PBOB *pbob                           = NULL;

  
  char *file_name                      = NULL;
  char *string1                        = NULL;

  int i                                = -9999999;
  int j                                = -9999999;
  int cluster_size                     = -9999999;
  int total_particles                  = -9999999;
  int start_indx                       = -9999999;
  int N                                = -9999999;
  int time_slice                       = -9999999;
  int time_slice_x                     = -9999999;
  int args_defined                     = -9999999;
  int exit_status                      = EXIT_FAILURE;

  file_name = (char *)calloc(MAX_CHARS,sizeof(char));
  args_defined = 0;
  i = 0;
  while(i<argc)
  {
    string1 = (char *)calloc(MAX_CHARS,sizeof(char));
    if(   (ContainsString(argv[i],(char *)"time_slice=")==TRUE) )
    {
      CopyAfterEqual(string1,argv[i]);
      time_slice = atoi(string1);
      args_defined++;
    }
    if(   (ContainsString(argv[i],(char *)"start_index=")==TRUE) )
    {
      CopyAfterEqual(string1,argv[i]);
      start_indx = atoi(string1);
      args_defined++;
    }
    if(   (ContainsString(argv[i],(char *)"N=")==TRUE) )
    {
      CopyAfterEqual(string1,argv[i]);
      N = atoi(string1);
      args_defined++;
    }
    if(   (ContainsString(argv[i],(char *)"file_name=")==TRUE) )
    {
      CopyAfterEqual(string1,argv[i]);
      sprintf(file_name,"%s",string1);
      args_defined++;
    }
    free(string1); string1 = NULL;
    i++;
  }
  if (args_defined != 4)
  {
    printf(" Usage: read_pbob_file time_slice=<int> start_index=<int> N=<int> file_name=<FILE NAME>\n");
    exit_status = EXIT_FAILURE;
    goto RETURN;
  }

  if ( (pbob=ReadPBOB(file_name))==NULL)
  {
    printf(" -unable to read PBOB struct, -aborting \n");
    exit_status = EXIT_FAILURE;
    goto RETURN;
  }
  if (pbob->endian_int == 2)
  {
    goto RETURN;
  }
  cluster_size = pbob->cluster_size;
  if ((node_descrip=ReadNodeDescrip(file_name,cluster_size))==NULL)
  {
    printf(" -unable to read NODE_DESCRIP struct, -aborting \n");
    exit_status = EXIT_FAILURE;
    goto RETURN;
  }
  
  printf("\n CLUSTER NAME:%s",pbob->cluster_name);
  printf("\n COMPUTE NODES: \n\n");
  printf("                           node      rank\n");
  printf("                         ========    =====\n");
  for (i=0;i<cluster_size;i++)
  {
    printf("%s    %s\n",node_descrip[i].node_name,node_descrip[i].node_number);
  }
  printf("\n");
  printf(" dimensions      = %i \n",pbob->dimensions);
  printf(" cluster_size    = %i \n",pbob->cluster_size);
  printf(" length_units    = %s \n",pbob->length_units);
  printf(" mass_units      = %s \n",pbob->mass_units);
  printf(" time_units      = %s \n",pbob->time_units);
  printf(" short title     = %s \n",pbob->short_title);
  printf(" energy_units    = %s \n",pbob->internal_energy_units);
  printf(" total_particles = %i \n",(int)pbob->total_particles);
  printf(" nn_k            = %i \n",pbob->nn_k);
  printf(" offset          = %i \n",pbob->first_particle_byte_offset);
  printf(" length          = %i \n",pbob->particle_length_bytes);
  printf(" endian          = %s \n",pbob->endian_str);
  printf(" version         = %s \n",pbob->pbob_version);
  printf(" slices          = %i \n",pbob->number_of_time_slices);
  printf(" kernel function: \n");
  printf("%s\n",pbob->kernel_function);
  printf(" ascii_header: \n");
  printf("%s\n",pbob->ascii_header);
  total_particles = (int)pbob->total_particles;

  printf("\n\n###############################  ALL TIME SLICES      #########################\n");
  time_slice_x = 1;
  while ( (particle=ReadParticle(file_name,time_slice_x,start_indx,N))!=NULL)
  {
/*
    for (j=0;j<N;j++)
    {
      printf("time slice = %i time = %8.5f x = %20.10f h = %20.10f\n",(int)particle[j].time_slice,particle[j].time,particle[j].x,particle[j].h);
    }
*/
    printf("time slice = %i time = %8.5f x = %20.10f h = %20.10f\n",(int)particle[0].time_slice,particle[0].time,particle[0].x,particle[0].h);
    time_slice_x++;
    free(particle); particle = NULL;
  }
  printf("\n\n################################ TIME SLICE = %i      #########################\n",time_slice);
  particle=ReadParticle(file_name,time_slice,start_indx,N);
  if (particle != NULL)
  {
    for (j=0;j<N;j++)
    {
      printf("time slice = %i time = %8.5f x = %20.10f z = %20.10f \n",(int)particle[j].time_slice,particle[j].time,particle[j].x,particle[j].z);
    }
  }

  exit_status = EXIT_SUCCESS;
RETURN:
  free(file_name);file_name = NULL;

  return 0;
}
/*---------------------------------------------------------------------------*\
|                                                                             |
| File:       contains_string.c                                               |
|                                                                             |
|                                                                             |
| Function:   logical ContainsString(char *string1,char *string2)             |
|                                                                             |
|                                                                             |
| Call:       ContainsString(string1,string1);                                |
|                                                                             |
| Purpose:    Determines if string1 contains string2                          |
|                                                                             |
|                                                                             |
| Input:      char *string1:  first string                                    |
|             char *string2:  second string                                   |
|                                                                             |
| Output:     None:                                                           |
|                                                                             |
| Returned    logical                                                         |
| Value:                                                                      |
|                                                                             |
| Global                                                                      |
| Variables:                                                                  |
|             None.                                                           |
|                                                                             |
| Required                                                                    |
| Resources:  Free_sph                                                        |
|                                                                             |
|-----------------------------------------------------------------------------|
| David Brown, : Feb 2011                                                     |
|                                                                             |
\*---------------------------------------------------------------------------*/

logical ContainsString(char *string1,char *string2)
{
  char *string3;

  int len_string1 = 0;
  int len_string2 = 0;
  int first = 0;
  int last = 0;

  logical result = FALSE;

  string3 = (char *)calloc(MAX_CHARS,sizeof(char));
  len_string2 = strlen(string2);
  len_string1 = strlen(string1);
  if (len_string1 < len_string2)
  {
    result = FALSE;
  }
  else
  {
    first = 0;
    last = first + len_string2;
    while(last <= len_string1)
    {
      memcpy((void *)string3,(void *)(string1+first),last-first);
      if (strcmp(string2,string3) == 0)
      {
        result = TRUE;
      }
      first++;
      last = first + len_string2;
    }
  }

  free(string3); string3 = NULL;
  return result;
}
/*---------------------------------------------------------------------------*\
|                                                                             |
| File:       copy_after_equal.c                                              |
|                                                                             |
|                                                                             |
| Function:   char *CopyAfterEqual(char *target,  char *source)               |
|                                                                             |
| Call:       CopyAfterEqual(string1,string1);                                |
|                                                                             |
| Purpose:    if source string contains an equal sign then return all         |
|             characters after the equal as a null-terminated string          |
|             will terminate at a \n                                          |
|                                                                             |
| Input:      char *target:  output string                                    |
|             char *source:  input string                                     |
|                                                                             |
| Output:     None:                                                           |
|                                                                             |
| Returned    char *target                                                    |
| Value:                                                                      |
|                                                                             |
| Global                                                                      |
| Variables:                                                                  |
|             None.                                                           |
|                                                                             |
| Required                                                                    |
| Resources:  Free_sph                                                        |
|                                                                             |
|-----------------------------------------------------------------------------|
| David Brown, : Feb 2011                                                     |
|                                                                             |
\*---------------------------------------------------------------------------*/

char *CopyAfterEqual(char *target,  char *source)
{   
  extern FILE *logfile_ptr;

  if (source == NULL)printf("CopyAfterEqual(): NULL source string passed to CopyAfterEqual\n");

  while ( *source != '\0')
  {
    while (*source != '=')
    {
      source++;
    }
    source++; /* Skip = */
    while(*source !='\0') 
    {
      *target = *source;
      target++;
      source++;
    }
  } 
  *target = '\0';
  return target;
}
