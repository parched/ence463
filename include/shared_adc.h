/**
 * \file shared_adc.h
 * \brief Common module allowing configuration and reading of ADC input.
 * \author James Duley
 * \version 1.0
 * \date 2014-08-21
 */

/* Copyright (C) 
 * 2014 - James Duley
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#ifndef SHARED_ADC_H
#define SHARED_ADC_H

#define ACC_SPRUNG_ADC 0x01       /**< The sprung acceleration ADC. */
#define ACC_UNSPRUNG_ADC 0x02     /**< The unsprung acceleration ADC. */
#define COIL_EXTENSION_ADC 0x04   /**< The coil extension ADC. */
#define ACTUATOR_FORCE_ADC 0x01   /**< The actuator force ADC. */
#define DAMPING_COEFF_ADC 0x02    /**< The damping coefficient ADC. */

/**
 * \brief Initialises the ADC module.
 *
 * \param adcs The bitwise OR of the ADCs to init.
 */
void initAdcModule(char adcs);

/**
 * \brief Gets a smoothed ADC value.
 *
 * \param adc The ADC to read.
 * \param minValue The 0V Return Value 
 * \param maxValue The 3V Return Value
 *
 * \return The smoothed value from the ADC.
 */
int getSmoothAdc(char adc, int minValue, int Maxvalue);

#endif

