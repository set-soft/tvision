/*************************************************************************/
/*                                                                       */
/* SSINPUTL.CPP                                                          */
/*                                                                       */
/* Copyright (c) 1992, Vincent J. Dentice                                */
/* All rights reserved                                                   */
/*                                                                       */
/* This file contains the stream registration object for the class       */
/* TStaticInputLine.                                                     */
/*                                                                       */
/*                                                                       */
/*   Date    Prg  Ver  Description                                       */
/* --------  ---  ---  ------------------------------------------------- */
/* 11/16/92  VJD  0.2  Added streamability to the TStaticInputLine class */
/*                                                                       */
/*************************************************************************/


#define Uses_TStaticInputLine
#define Uses_TStreamableClass
#include "tsinputl.h"
__link( RInputLine )


TStreamableClass RStaticInputLine( TStaticInputLine::name,
				   TStaticInputLine::build,
				   __DELTA(TStaticInputLine)
				 );
