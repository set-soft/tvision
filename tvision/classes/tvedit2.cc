/* Modified by Robert Hoehne and Salvador Eduardo Tropea for the gcc port */
/* Modified to compile with gcc v3.x by Salvador E. Tropea, with the help of
   Andris Pavenis. */
/* Modified for i18n support by Salvador Eduardo Tropea. */
/*----------------------------------------------------------*/
/*                                                          */
/*   Turbo Vision 1.0                                       */
/*   Copyright (c) 1991 by Borland International            */
/*                                                          */
/*   Turbo Vision TVEDIT source file                        */
/*----------------------------------------------------------*/

#define Uses_stdlib
#define Uses_stdarg

#define Uses_TDialog
#define Uses_TDeskTop
#define Uses_TProgram
#define Uses_TApplication
#define Uses_TObject
#define Uses_TInputLine
#define Uses_T1Label
#define Uses_THistory
#define Uses_TRect
#define Uses_TCheckBoxes
#define Uses_TButton
#define Uses_MsgBox
#define Uses_TSItem
#define Uses_TEditor
#define Uses_TEditorApp

#include <tv.h>

ushort execDialog( TDialog *d, void *data )
{
    // SET: set the data before checking if the dialog is valid.
    // If we use validators that's very important.
    if( d && data )
        d->setData( data );
    TView *p = TProgram::application->validView( d );
    if( p == 0 )
        return cmCancel;
    else
        {
        ushort result = TProgram::deskTop->execView( p );
        if( result != cmCancel && data != 0 )
            p->getData( data );
        TObject::CLY_destroy( p );
        return result;
        }
}

TDialog *createFindDialog()
{
    TDialog *d = new TDialog( TRect( 0, 0, 38, 12 ), __("Find") );

    d->options |= ofCentered;

    TInputLine *control = new TInputLine( TRect( 3, 3, 32, 4 ), 80 );
    d->insert( control );
    d->insert(
        new T1Label( 2, 2, __("~T~ext to find"), control ) );
    d->insert(
        new THistory( TRect( 32, 3, 35, 4 ), control, 10 ) );

    d->insert( new TCheckBoxes( TRect( 3, 5, 35, 7 ),
        new TSItem( __("~C~ase sensitive"),
        new TSItem( __("~W~hole words only"), 0 ))));

    d->insert(
        new TButton( TRect( 14, 9, 24, 11 ), __("O~K~"), cmOK, bfDefault ) );
    d->insert(
        new TButton( TRect( 26, 9, 36, 11 ), __("Cancel"), cmCancel, bfNormal ) );

    d->selectNext( False );
    return d;
}

TDialog *createReplaceDialog()
{
    TDialog *d = new TDialog( TRect( 0, 0, 40, 16 ), __("Replace") );

    d->options |= ofCentered;

    TInputLine *control = new TInputLine( TRect( 3, 3, 34, 4 ), 80 );
    d->insert( control );
    d->insert(
        new T1Label( 2, 2, __("~T~ext to find"), control ) );
    d->insert( new THistory( TRect( 34, 3, 37, 4 ), control, 10 ) );

    control = new TInputLine( TRect( 3, 6, 34, 7 ), 80 );
    d->insert( control );
    d->insert( new T1Label( 2, 5, __("~N~ew text"), control ) );
    d->insert( new THistory( TRect( 34, 6, 37, 7 ), control, 11 ) );

    d->insert( new TCheckBoxes( TRect( 3, 8, 37, 12 ),
        new TSItem(__("~C~ase sensitive"),
        new TSItem(__("~W~hole words only"),
        new TSItem(__("~P~rompt on replace"),
        new TSItem(__("~R~eplace all"), 0 ))))));

    d->insert(
        new TButton( TRect( 17, 13, 27, 15 ), __("O~K~"), cmOK, bfDefault ) );
    d->insert( new TButton( TRect( 28, 13, 38, 15 ),
                            __("Cancel"), cmCancel, bfNormal ) );

    d->selectNext( False );

    return d;
}

