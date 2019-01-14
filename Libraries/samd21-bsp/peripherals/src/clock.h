/**************************************************************
 *                                                            *
 *               Copyright (c) 2019 - Patrick Rohr            *
 *                      All Rights Reserved                   *
 *                                                            *
 *************************************************************/

#pragma once

/*!************************************************************
 * @brief      Initialize, must be called before using any clock source.
 * @date       January 13, 2019
 * @author     Patrick Rohr
**************************************************************/
void clock_init();

/*!************************************************************
 * @brief      Start OSC32K
 * @date       January 13, 2019
 * @author     Patrick Rohr
**************************************************************/
void clock_osc32k_start();

/*!************************************************************
 * @brief      Stop OSC32K
 * @date       January 13, 2019
 * @author     Patrick Rohr
**************************************************************/
void clock_osc32k_stop();

/*!************************************************************
 * @brief      Start DFLL48M
 * @date       January 13, 2019
 * @author     Patrick Rohr
**************************************************************/
void clock_dfll48m_start();

/*!************************************************************
 * @brief      Stop DFLL48M
 * @date       January 13, 2019
 * @author     Patrick Rohr
**************************************************************/
void clock_dfll48m_stop();

/*!************************************************************
 * @brief      Start XOSC32K
 * @date       January 13, 2019
 * @author     Patrick Rohr
**************************************************************/
void clock_xosc32k_start();

/*!************************************************************
 * @brief      Stop XOSC32K
 * @date       January 13, 2019
 * @author     Patrick Rohr
**************************************************************/
void clock_xosc32k_stop();