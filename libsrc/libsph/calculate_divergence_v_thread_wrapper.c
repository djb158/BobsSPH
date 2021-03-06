/*---------------------------------------------------------------------------*\
|                                                                             |
|  File: 
|                                                                             |
|  Function: 
|                                                                             |
|  Purpose:  
|            
|                                                                             |
|  Call:                                                                      |
|           
|                                                                             |
|  Input:   
|           
|                                                                             |
|                                                                             |
|  Output:  
|          
|                                                                             |
|  Global variables:  
|                                                                             |
|  Required resources:  
|                                                                             |
|  Error:  
|-----------------------------------------------------------------------------|
|                                                                             |
|  David Brown 29-Jul-2015   initial version                                  |
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

#include "calculate_divergence_v_thread_wrapper.h"

int CalculateDivergenceV_thread_wrapper(PARTICLES *particles,PARTICLES *x_particles,PARS *pars)
{
  extern NODE_INFO *node_info;

  pthread_t *thread                    = NULL;
  THREAD_DATA *data                    = NULL;

  char   fname[]                       = "...CalculateParticleDivergeceV_thread_wrapper";

  int i                                = LARGE_NEGATIVE_INT;
  int tasks_per_thread                 = LARGE_NEGATIVE_INT;
  int n_interior_and_boundary          = LARGE_NEGATIVE_INT;
  int nparticles                       = LARGE_NEGATIVE_INT;
  int exit_status                      = EXIT_FAILURE;
  int *err                             = NULL;

#include "pars_include.h"
  verbosity(1,fname,pars);

  nparticles       = node_info[rank].nparticles;
  tasks_per_thread = (nparticles+COMPUTE_THREADS-1)/COMPUTE_THREADS;
  data             = (THREAD_DATA *)calloc(COMPUTE_THREADS,sizeof(THREAD_DATA));
  thread           = (pthread_t *)calloc(COMPUTE_THREADS,sizeof(pthread_t));
  err              = (int *)calloc(COMPUTE_THREADS,sizeof(int));

  for (i=0; i<COMPUTE_THREADS; i++)
  {
    data[i].exit_status = &exit_status;
    data[i].start       = i*tasks_per_thread;
    data[i].stop        = (i+1)*tasks_per_thread;
    data[i].particles   = particles;
    data[i].x_particles = x_particles;
    data[i].pars        = pars;
    data[i].matrix      = NULL;
    data[i].vector      = NULL;
    err[i]              = LARGE_NEGATIVE_INT;
  }
  data[COMPUTE_THREADS-1].stop = nparticles;

  for (i=0; i<COMPUTE_THREADS; i++)
  {
    err[i] = pthread_create(&thread[i], NULL,CalculateDivergenceV, data+i);
  }

  for (i=0; i<COMPUTE_THREADS; i++)
  {
    pthread_join(thread[i], NULL);
  }

  for (i=0; i<COMPUTE_THREADS; i++)
  {
    if ( (*data[i].exit_status == EXIT_FAILURE) || (err[i] != 0) )
    {
      exit_status = EXIT_FAILURE;
      printf(" Rank: %i  %s() -error return from pthread_create for CalculateDivergenceV() \n",rank,fname);
      goto RETURN;
    }
  }

  for (i=0; i<COMPUTE_THREADS; i++)
  {
    data[i].particles   = NULL;
    data[i].x_particles = NULL;
    data[i].pars        = NULL;
    data[i].matrix      = NULL;
    data[i].vector      = NULL;
  }
  Free_sph(thread);
  Free_sph(data);
  Free_sph(err);

  exit_status = EXIT_SUCCESS;
RETURN:
  verbosity(2,fname,pars);
  free(err);
  return exit_status;
}
