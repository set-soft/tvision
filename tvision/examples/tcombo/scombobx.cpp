/*************************************************************************/
/*                                                                       */
/* SCOMBOBX.CPP                                                          */
/*                                                                       */
/* Copyright (c) 1992, Vincent J. Dentice                                */
/* All rights reserved                                                   */
/*                                                                       */
/* This file contains the stream registration object for the class       */
/* TComboBox.                                                            */
/*                                                                       */
/*                                                                       */
/*   Date    Prg  Ver  Description                                       */
/* --------  ---  ---  ------------------------------------------------- */
/* 11/16/92  VJD  0.2  Added streamability to the TComboBox classes.     */
/*                                                                       */
/*************************************************************************/


#define Uses_TComboBox
#define Uses_TStreamableClass
#include "tcombobx.h"
__link( RView )


TStreamableClass RComboBox( TComboBox::name,
			    TComboBox::build,
			    __DELTA(TComboBox)
			  );
