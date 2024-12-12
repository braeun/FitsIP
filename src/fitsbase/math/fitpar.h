/********************************************************************************
 *                                                                              *
 * Fit parameter definition                                                     *
 *                                                                              *
 * modified: 2022-01-27                                                         *
 *                                                                              *
 ********************************************************************************
 * Copyright (C) 2005-2008 by Harald Braeuning.   All Rights Reserved.          *
 ********************************************************************************
 * This program is free software; you can redistribute it and/or                *
 * modify it under the terms of the GNU General Public License                  *
 * as published by the Free Software Foundation; either version 2.1             *
 * of the License, or (at your option) any later version.    	                  *
 *                                                                              *
 * This program is distributed in the hope that it will be useful,              *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of               *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU            *
 * General Public License for more details.                                     *
 *                                                                              *
 * You should have received a copy of the GNU General Public                    *
 * License along with this library; if not, write to the Free Software          *
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.   *
 ********************************************************************************/

#ifndef HMATH_FITPAR_H
#define HMATH_FITPAR_H

/**
 * @brief This class contains all information about one fit parameter
 */
class FitPar
{
 public:
  FitPar(void);

  double value;     /**< value of parameter */
  double lo_bound;  /**< lower boundary for parameter */
  double hi_bound;  /**< higher boundary for parameter */
  bool bounded;     /**< if true, the parameter will only be varied between @a _lo_bound and @a _hi_bound */
  bool variable;    /**< if false, the parameter will not be varied */
};

#endif
