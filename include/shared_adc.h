/**
 * \file shared_adc.h
 * \brief Active suspension ADC module.
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

#define ACC_SPRUNG_ADC 0x01
#define ACC_UNSPRUNG_ADC 0x02
#define COIL_EXTENSION_ADC 0x04
#define ACTUATOR_FORCE_ADC 0x01
#define DAMPING_COEFF_ADC 0x02

/**
 * \brief Initialises the ADC module.
 *
 * \param adcs The ADCs of which to init, OR of some XXXXX_ADC.
 */
void initAdcModule(char adcs);

/**
 * \brief Gets a smoothed ADC value.
 *
 * \param adc The ADC to read, one of XXXX_ADC.
 *
 * \return The smoothed value from the ADC.
 */
int getSmoothAdc(char adc);

#endif
