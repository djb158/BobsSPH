/*----------------------------------------------------------------------------|
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

#ifndef BOBSSPH_CALCULATE_PARTICLE_ACCELERATION_INTERIOR_H
#define BOBSSPH_CALCULATE_PARTICLE_ACCELERATION_INTERIOR_H

#include "sph_main.h"
#include <pthread.h>
#include "pars.h"
#include "particles.h"
#include "numeric_constants.h"
#include "thread_data.h"

extern void verbosity(int k, char fname[],PARS *pars);
extern double dWdf(double f,double h,int dim,double kernel_width);
extern double d2Wdf2(double f,double h,int dim,double kernel_width);
extern double dfdx(double x1,double x2,double f);
extern double dfdy(double y1,double y2,double f);
extern double dfdz(double z1,double z2,double f);

void *CalculateParticleAcceleration_INTERIOR(void *acd);

#endif
