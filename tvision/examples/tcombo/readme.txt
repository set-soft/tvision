The included files are Turbo Vision source code files designed to
create a Combo Box similar in functionality to the one in Microsoft
Windows.  All code in these files is considered by the author to be
part of the public domain.

The following files should be included with this file:
     README.TXT     This file.
     TCOMBOBX.TXT   TComboBox documentation in ASCII format.
     TCOMBOBX.WP5   TComboBox documentation in WordPerfect 5.1 format.
     TCOMBOBX.H     TComboBox header file.
     TCOMBOBX.CPP   TComboBox source code.
     SCOMBOBX.CPP   Stream registration code for TComboBox.
     NMCMBOBX.CPP   Name registration code for TComboBox.
     TCMBOVWR.CPP   TComboViewer source code.
     SCMBOVWR.CPP   Stream registration code for TComboViewer.
     NMCMBOVW.CPP   Name registration code for TComboViewer.
     TCMBOWIN.CPP   TComboWindow source code.
     SCMBOWIN.CPP   Stream registration code for TComboWindow.
     NMCMBOWN.CPP   Name registration code for TComboWindow.
     TSINPUTL.H     TStaticInputLine header file.
     TSINPUTL.CPP   TStaticInputLine source code.
     SSINPUTL.CPP   Stream registration code for TStaticInputLine.
     NMSINPUT.CPP   Name registration code for TStaticInputLine.
     TEST.CPP       Example program using TComboBox and TStaticInputLine.

I have added streamability to all classes.  I'm not certain that TComboViewer
or TComboWindow would ever be saved to a stream, since they are not actually
part of the dialog box, but I added the functionality just in case.

Some additional changes that have been made are:
     1)  Added the getData, setData, and dataSize functions to TComboBox.
         This provides the programmer with the additional ability of setting
         the TComboBox options through a setData call to the dialog box.
     2)  Added the missing dataSize function to TComboViewer.  By not over-
         ridding this function, a data size of 0 was returned and caused the
         next object in the dialog box to over-write the TComboViewer record.
     3)  Added code to highlight the currently selected item when the combo
         box is opened.  This code was provided by Robert Gloeckner.
     4)  Added code to allow the user to use the Home and End keys in the
         TStaticInputLine to go to the beginning and end of the list
         respectively.  This code was also provided by Robert Gloeckner.

I welcome comments and suggestions, and hope that this source code proves
to be useful.

I would also like to say thanks to Robert Gloeckner (100034,3033) for his
contributions.

Vincent Dentice
70413,1404

