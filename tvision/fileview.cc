/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified to compile with gcc v3.x by Salvador E. Tropea, with the help of
Andris Pavenis.
Modified by Salvador E. Tropea to compile for 64 bits architectures.

 *
 *
 */
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#define Uses_stdio
#define Uses_string
#define Uses_stdlib
#define Uses_ctype
#define Uses_fcntl
#define Uses_sys_stat
#define Uses_filelength
#define Uses_unistd
#define Uses_AllocLocal

#define Uses_MsgBox
#define Uses_TKeys
#define Uses_TScroller
#define Uses_TDrawBuffer
#define Uses_TRect
#define Uses_TProgram
#define Uses_TDeskTop
#define Uses_TStreamableClass
#define Uses_TFileViewer
#define Uses_TFileDialog
#include <tv.h>

#if !defined( NO_STREAM )

__link(RScroller)
__link(RScrollBar)

const char * const TFileViewer::name = "TFileViewer";

TStreamableClass RFileView( TFileViewer::name,
                            TFileViewer::build,
                              __DELTA(TFileViewer)
                          );

void *TFileViewer::read(ipstream& is)
{
    char *fName;

    TScroller::read(is);
    fName = is.readString();
    fileName = NULL;
    readFile(fName);
    delete fName; 
    return this;
}

void TFileViewer::write(opstream& os)
{
    TScroller::write(os);
    os.writeString(fileName);
}

TStreamable *TFileViewer::build()
{
    return new TFileViewer( streamableInit );
}

#endif // NO_STREAM

void TFileViewer::changeBounds(const TRect & bounds)
{
  TScroller::changeBounds(bounds);
  setLimit(size.x + width,size.y + count);
}

TFileViewer::TFileViewer( const TRect& bounds,
                          TScrollBar *aHScrollBar,
                          TScrollBar *aVScrollBar,
                          const char *aFileName) :
    TScroller( bounds, aHScrollBar, aVScrollBar ),
    buffer(NULL), real_bufsize(0), width(0), count(0)
{
    growMode = gfGrowHiX | gfGrowHiY;
    isValid = True;
    fileName = 0;
    helpCtx = hcFileViewer;
    readFile( aFileName );
    delta.x = delta.y = 0;
}

TFileViewer::~TFileViewer()
{
     if (buffer) free(buffer);
     delete[] fileName;
     fileLines->removeAll();
     CLY_destroy(fileLines);
}

void TFileViewer::draw()
{
    const char *p;
    AllocLocalStr(s,size.x+1);

    ushort c =  getColor(0x0301);
    for( int i = 0; i < size.y; i++ )
        {
        TDrawBuffer b;
        b.moveChar( 0, ' ', c, size.x );

        if( delta.y + i < fileLines->getCount() )
            {
            p = operator[](delta.y+i);
            if( p == 0 || strlen(p) < (unsigned)delta.x )
                s[0] = EOS;
            else
                {
                strncpy( s, p+delta.x, size.x );
                s[size.x] = EOS;
                }
            b.moveStr( 0, s, c );
            }
        writeBuf( 0, i, size.x, 1, b );
        }
}

void TFileViewer::scrollDraw()
{
    TScroller::scrollDraw();
    draw();
}

void TFileViewer::readFile( const char *fName )
{
  if (fileName) delete[] fileName;

  limit.x = 0;
  fileName = newStr( fName );
  int fileToView = open(fName, O_RDONLY | O_TEXT, 0);
  if (fileToView <= 0)
  {
    messageBox(mfError | mfOKButton ,
               _("Could not open '%s' for reading"),fName);
    isValid = False;
  }
  else
  {
    size_t bufsize = filelength(fileToView) + 1;
    buffer = (char *)malloc(bufsize);
    real_bufsize = ::read(fileToView,buffer,bufsize);
    char *start,*end,*bufend = buffer + real_bufsize;
    start = buffer;
    while (start < bufend)
    {
      end = start;
      while (end < bufend && *end != '\n') end++;
      *end = 0;
      width = max( (size_t)width, strlen( start ) );
      count++;
      start = end + 1;
    }
    int i;
    fileLines = new TLineCollection(count, 5);
    fileLines->count = count;
    for (i=0,start = buffer;i<count;i++)
    {
      fileLines->items[i] = (void *)(start-buffer);
      while (*start++);
    }
    isValid = True;
  }
  count = fileLines->getCount();
  setLimit(size.x + width, size.y + count);
  close(fileToView);
}

void TFileViewer::saveFile(const char *fname)
{
  FILE *f;
  f = fopen(fname,"wt+");
  for (int i=0;i<count;i++)
    fprintf(f,"%s\n",operator[](i));
  fclose(f);
}

void TFileViewer::handleEvent(TEvent &event)
{
  TScroller::handleEvent(event);
  switch (event.what)
  {
    case evBroadcast:
      switch (event.message.command)
      {
        case cmFileViewAnswer:
          clearEvent(event);
          break;
        default:
          break;
      }
      break;
    case evCommand:
      switch (event.message.command)
      {
        case cmSaveFile:
        {
          char fname[256];
          TFileDialog *dialog;
          dialog = new TFileDialog("*",_("Save to file"),
                                   _("~N~ame"),fdOKButton,0);
          if (TProgram::deskTop->execView(dialog) != cmCancel)
          {
            dialog->getData(fname);
            saveFile(fname);
          }
          CLY_destroy(dialog);
          break;
        }
        default:
          break;
      }
      break;
    default:
      break;
  }
}

void TFileViewer::insertLine(const char *line)
{
  size_t len = strlen(line)+1;
  buffer = (char *)realloc(buffer,real_bufsize+len);
  memcpy(buffer+real_bufsize,line,len);
  fileLines->insert((void*)(uipointer)real_bufsize);
  real_bufsize += len;
  count++;
  len--;
  width = width < (int)len ? (int)len : width;
  setLimit(size.x + width, size.y + count);
}

void TFileViewer::setState( ushort aState, Boolean enable )
{
    TScroller::setState( aState, enable );
    if( enable && (aState & sfExposed) )
        setLimit( limit.x, limit.y );
}

Boolean TFileViewer::valid( ushort )
{
    return isValid;
}

const char * const TFileViewer::operator [](int index)
{
  if (index >= count) return NULL;
  return (const char *)(buffer + (uipointer)fileLines->at(index));
}




static int winNumber = 0;

TFileWindow::TFileWindow( const char *fileName ) :
    TWindowInit( &TFileWindow::initFrame ),
    TWindow( TProgram::deskTop->getExtent(), fileName, winNumber++ )
{
    options |= ofTileable;
    TRect r( getExtent() );
    r.grow(-1, -1);
    insert(new TFileViewer( r,
                            standardScrollBar(sbHorizontal | sbHandleKeyboard),
                            standardScrollBar(sbVertical | sbHandleKeyboard),
                            fileName) );
}


