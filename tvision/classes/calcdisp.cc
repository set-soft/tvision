/*------------------------------------------------------------*/
/*                                                            */
/*   Turbo Vision 1.0                                         */
/*   Copyright (c) 1991 by Borland International              */
/*                                                            */
/*   Calc.cpp:  TCalcDisplay member functions                 */
/*                                                            */
/*------------------------------------------------------------*/

/*
  Modified by Salvador Eduardo Tropea <salvador@inti.gov.ar>
  <set@ieee.org> <set@computer.org>
  I ported it from TV 1.03.
  SET: Added locale support for the decimal point.
 */

// SET: moved the standard headers before tv.h
#define Uses_string
#define Uses_stdlib
#define Uses_ctype
#define Uses_stdio // sprintf
#define Uses_nl_langinfo

#define Uses_TKeys
#define Uses_TKeys_Extended
#define Uses_TRect
#define Uses_TEvent
#define Uses_TButton
#define Uses_TKeys
#define Uses_TDrawBuffer
#define Uses_TStreamableClass
#define Uses_TStreamable
#define Uses_TView
#define Uses_TPalette // SET: added
#define Uses_TCalcDisplay
#include <tv.h>

#define cpCalcPalette   "\x13"

TCalcDisplay::TCalcDisplay(TRect& r) : TView ( r )
{
    options |= ofSelectable;
    eventMask = (evKeyboard | evBroadcast);
    number = new char[DISPLAYLEN];
    clear();

}

TCalcDisplay::~TCalcDisplay()
{
    DeleteArray(number);
}

TPalette& TCalcDisplay::getPalette() const
{
    static TPalette palette( cpCalcPalette, sizeof(cpCalcPalette)-1 );
    return palette;
}


void TCalcDisplay::handleEvent(TEvent& event)
{
    // SET: Independent of the label
    static char keys[]={'C','\x8','%','_','7','8','9','/','4','5','6',
                        '*','1','2','3','-','0','.','=','+'};
    TView::handleEvent(event);

    switch(event.what)
        {
        case evKeyboard:
            calcKey(event.keyDown.charScan.charCode,event.keyDown.keyCode);
            clearEvent(event);
            break;
        case evBroadcast:
            if(event.message.command>=cmCalcButton &&
               event.message.command<=cmCalcButton+19)
                {
                calcKey(keys[event.message.command-cmCalcButton],0);
                clearEvent(event);
                }
            break;
        }
}


void TCalcDisplay::draw()
{
    char color = getColor(1);
    int i;
    TDrawBuffer buf;

    i = size.x - strlen(number) - 2;
    buf.moveChar(0, ' ', color, size.x);
    buf.moveChar(i, sign, color, 1);
    buf.moveStr(i+1, number, color);
    writeLine(0, 0, size.x, 1, buf);
}


void TCalcDisplay::error()
{
    status = csError;
    strcpy(number, _("Error"));
    sign = ' ';
}


void TCalcDisplay::clear()
{
    status = csFirst;
    strcpy(number, "0");
    sign = ' ';
    operate = '=';
}


void TCalcDisplay::setDisplay(double r)
{
    int  len;
    char str[64];
    //ostrstream displayStr( str, sizeof str );SET: Removed this waste

    if(r < 0.0)
        {
        sign = '-';
        sprintf(str,"%f",-r);
        }
    else
        {
        sprintf(str,"%f",r);
        sign = ' ';
        }

    len = strlen(str) - 1;          // Minus one so we can use as an index.

    if(len > DISPLAYLEN)
        error();
    else
        strcpy(number, str);
}


void TCalcDisplay::checkFirst()
{
    if( status == csFirst)
        {
        status = csValid;
        strcpy(number, "0");
        sign = ' ';
        }
}


void TCalcDisplay::calcKey(unsigned char key, unsigned code)
{
    char stub[2] = " ";
    double r;
    char *decPoint=nl_langinfo(RADIXCHAR);

    switch(code)
        {
        case kbBackSpace:
             key=8;
             break;
        case kbEsc:
             key=27;
             break;
        case kbEnter: // Added by Mike
             key=13;
             break;
        }
    
    key = (unsigned char)toupper(key);
    if( status == csError && key != 'C')
        key = ' ';

    switch(key)
        {
        case '0':   case '1':   case '2':   case '3':   case '4':
        case '5':   case '6':   case '7':   case '8':   case '9':
            checkFirst();
            if (strlen(number) < 15) 
                {                       // 15 is max visible display length
                if (!strcmp(number, "0"))
                    number[0] = '\0';
                stub[0] = key;
                strcat(number, stub);
                }
            break;

        case 8:
        case 27:
            int len;

            checkFirst();
            if( (len = strlen(number)) == 1 )
                strcpy(number, "0");
            else
                number[len-1] = '\0';
            break;

        case '_': // +-
            sign = (sign == ' ') ? '-' : ' ';
            break;

        case '.':
             checkFirst();
             if(strstr(number, decPoint) == NULL)
                 strcat(number, decPoint);
            break;

        case '+':   case '-':   case '*':   case '/':
        case '=':   case '%':   case 13:
            if(status == csValid)
                {
                status = csFirst;
                r = getDisplay() * ((sign == '-') ? -1.0 : 1.0);
                if( key == '%' )
                    {
                    if(operate == '+' || operate == '-')
                        r = (operand * r) / 100;
                    else
                        r /= 100;
                    }
                switch( operate )
                    {
                    case '+':
                        setDisplay(operand + r);
                        break;

                    case '-':
                        setDisplay(operand - r);
                        break;

                    case '*':
                        setDisplay(operand * r);
                        break;

                    case '/':
                        if(r == 0)
                            error();
                        else
                            setDisplay(operand / r);
                        break;

                    }
                }
            operate = key;
            operand = getDisplay() * ((sign == '-') ? -1.0 : 1.0);
            break;

        case 'C':
            clear();
            break;

        }
    drawView();
}

#if !defined( NO_STREAM )
TStreamable *TCalcDisplay::build()
{
    return new TCalcDisplay( streamableInit );
}

void TCalcDisplay::write( opstream& os )
{
    TView::write( os );
    os.writeBytes(&status, sizeof(status));
    os.writeString(number);
    os.writeByte(sign);
    os.writeByte(operate);
    os.writeBytes(&operand, sizeof(operand));
}


void *TCalcDisplay::read( ipstream& is )
{
    TView::read( is );
    number = new char[DISPLAYLEN];
    is.readBytes(&status, sizeof(status));
    is.readString(number, DISPLAYLEN);
    sign = is.readByte();
    operate = is.readByte();
    is.readBytes(&operand, sizeof(operand));
    return this;
}
#endif // NO_STREAM



