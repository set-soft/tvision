/**[txh]********************************************************************

  Copyright (c) 2002 by Jan Van Buggenhout <Chipzz@ULYSSIS.Org>

  Based on code:
  Copyright (c) 1994 by Borland International
  Modified by Sergio Sigala <ssigala@globalnet.it>
  Modified by Salvador E. Tropea <set@ieee.org>, <set@users.sourceforge.net>
  Copyright (c) 1999 by Andreas F. Gorup von Besanez <besanez@compuserve.de>

  Covered by the GPL license.

  Description:
  This is a class derived from THelpFile to read Borland help files.
  
***************************************************************************/

#define Uses_ctype
#define Uses_limits
#define Uses_string
#define Uses_stdlib
#define Uses_sys_stat
#define Uses_filelength
#define Uses_TCollection
#define Uses_TStreamableClass
#define Uses_THelpFile
#define Uses_fpstream
#include <tv.h>
#include "bhelp.h"

char *read0String(fpstream & s)
{
  int l;
  long p = s.tellg();

  for (l = 1; s.readByte(); l++) ;
  char *buffer = new char[l];

  s.seekg(p);
  s.readBytes(buffer, l);
  return buffer;
}

// TBorlandHelpTopic

const char *const TBorlandHelpTopic::name = "TBorlandHelpTopic";

TBorlandHelpTopic::TBorlandHelpTopic(fpstream & s, char *comp)
{
  char type;
  unsigned short datalength, refslength;

  s >> type;
  s >> datalength;
  unsigned char *data = new unsigned char[datalength];
  char *text = new char[datalength * 9];
  char *curtext = text;

  s.readBytes(data, datalength);

  ushort noRefs;

  s >> type;
  s >> refslength;
  s >> previous;
  s >> next;
  s >> noRefs;
  setNumCrossRefs(0);
  refslength -= 6;
  ushort *refs = (ushort *)(new uchar[refslength]);
  ushort *curref = refs;

  s.readBytes(refs, refslength);

  for (long k = 0; k < datalength * 2; k++)
  {
    unsigned char Nibble;

    Nibble = (data[k / 2] >> ((k & 1) * 4)) & 0xf;
    switch (Nibble)
    {

      case 0:
        *curtext++ = 0;
        break;

      case 0xe:                /* Rep */
      {
        ushort RepCount;

        k++;
        RepCount = ((data[k / 2] >> ((k & 1) * 4)) & 0xf) + 2;
        k++;
        Nibble = (data[k / 2] >> ((k & 1) * 4)) & 0xf;
        if (Nibble < 0xe)
        {
          memset(curtext, comp[Nibble], RepCount);
          curtext += RepCount;
        }
        else if (Nibble == 0xf)
        {
          k++;
          Nibble = data[k / 2] >> ((k & 1) * 4);
          k++;
          memset(curtext,
                 Nibble | ((data[k / 2] >> ((k & 1) * 4)) << 4), RepCount);
          curtext += RepCount;
        }
        break;
      }

      case 0xf:                /* Raw */
        k++;
        Nibble = data[k / 2] >> ((k & 1) * 4);
        k++;
        *curtext++ = Nibble | ((data[k / 2] >> ((k & 1) * 4)) << 4);
        break;

      default:
        *curtext++ = comp[Nibble];
        break;
    }
  }

  for (char *para = text; (para < curtext) && (*para != 1);
       para += strlen(para) + 1)
  {
    for (char *ref = strchr(para, 2); ref; ref = strchr(ref, 2))
    {
      char *endref = strchr(ref + 1, 2) - 1;
      int length = endref - ref;

      TCrossRef cref;

      cref.offset = ref - text + 1;
      cref.length = length;
      cref.ref = *curref++;
      addCrossRef(cref);

      memmove(ref, ref + 1, length);
      memmove(endref, endref + 2, curtext - endref);

      curtext -= 2;
    }
    TParagraph *paragraph = new TParagraph;

    paragraph->text = strdup(para);
    paragraph->size = strlen(para) + 1;
    paragraph->wrap = False;
    addParagraph(paragraph);
  }
  free(data);
  free(text);
  free(refs);
}

TStreamableClass RBorlandHelpTopic(TBorlandHelpTopic::name,
                                   TBorlandHelpTopic::build,
                                   __DELTA(TBorlandHelpTopic));

// TBorlandHelpIndex

const char *const TBorlandHelpIndex::name = "TBorlandHelpIndex";

TBorlandHelpIndex::TBorlandHelpIndex(fpstream & s):THelpIndex()
{
  unsigned short length;
  unsigned char *buffer;

  s >> length;
  buffer = new unsigned char[length];

  s.readBytes(buffer, length);
  size = *((unsigned short *) buffer);
  index = new long[size];

  buffer += 2;
  for (int k = 0; k < size * 3; k += 3)
    index[k/3] = ((buffer[k + 2] & 0x80) << 24) +
      (buffer[k] + (buffer[k + 1] << 8) + ((buffer[k + 2] & 0x7f) << 16));
}

void *TBorlandHelpIndex::read(ipstream & is)
{
  // FIXME
  return 0;
}

TStreamable *TBorlandHelpIndex::build()
{
  return new TBorlandHelpIndex(streamableInit);
}

TStreamableClass RBorlandHelpIndex(TBorlandHelpIndex::name,
                                   TBorlandHelpIndex::build,
                                   __DELTA(TBorlandHelpIndex));

// TBorlandHelpFile

TBorlandHelpFile::TBorlandHelpFile(fpstream & s):THelpFile(s)
{
  char *Stamp;
  char FormatVersion;

  modified=False;
  stream->seekg(0);
  Stamp = read0String(*stream);
  stream->seekg(14, CLY_IOSCur);
  *stream >> FormatVersion;
  stream->seekg(1, CLY_IOSCur);

  while (!stream->eof())
  {
    char type;
    unsigned short length;
    unsigned char *buffer=NULL;

    *stream >> type;
    if (type != 1)
    {
      *stream >> length;
      buffer = (unsigned char *) malloc(length);
      stream->readBytes(buffer, length);
    }
    switch (type)
    {
      case 0:                  /* FileHeader */
      {
        ushort *o = (ushort *) buffer;
        options = o[0];
        indexscreen = o[1];
        height = buffer[6];
        width  = buffer[7];
        margin = buffer[8];
        break;
      }

      case 1:                  /* ConText */
        index = new TBorlandHelpIndex(*stream);
        break;

      case 2:                  /* Text */
        stream->seekg(-(length + 3), CLY_IOSCur);
        return;

      case 3:                  /* KeyWord */
        stream->seekg(-(length + 3), CLY_IOSCur);
        return;

      case 4:                  /* Index */
      {
        char temp[41];
        *temp = 0;
        glossaryItems = *((ushort *) buffer);
        glossary = new char *[glossaryItems];
        char *g = (char *) buffer + 2;
        for (int i = 0; i < glossaryItems; i++)
        {
          uchar lengthcode = *g++;
          ushort carry = (lengthcode & 0xe0) >> 5;
          ushort unique = lengthcode & 0x1f;
          glossary[i] = new char[carry + unique + 3];
          strncpy(temp + carry, g, unique);
          temp[carry + unique] = 0;
          g += unique;
          strncpy(temp + carry + unique + 1, g, 2);
          g += 2;
          memcpy(glossary[i], temp, carry + unique + 3);
        }
        break;
      }

      case 5:                  /* Compression */
        *compressiontable = 0;
        memcpy(compressiontable + 1, buffer + 2, 13);
        break;

      case 6:                  /* IndexTags */
        break;
    }
    if (type != 1)
      free(buffer);
  }
}

class TSearchResults : public TCollection
{
  public:
  TSearchResults() : TCollection(0, 1) {};
  virtual void *readItem( ipstream& ) { return 0; };
  virtual void writeItem( void *, opstream& ) {};
};

TCollection *TBorlandHelpFile::search(char *s)
{
  char *ss = strdup(s);
  if (!(options & 4)) strupr(ss);
  int found = 0;
  int length = strlen(s);
  TCollection *coll = new TSearchResults();
  for (int i = 0; i < glossaryItems; i++)
  {
    if (!strncmp(ss, glossary[i], length))
    {
      found = 1;
      coll->insert(glossary[i]);
    }
    else if (found) break;
  }
  free(ss);
  return coll;
}

THelpTopic *TBorlandHelpFile::getTopic(int i)
{
  long pos = index->position(i);

  if (pos > 0)
  {
    stream->seekg(pos);
    return new TBorlandHelpTopic(*stream, compressiontable);
  }
  else
    return (invalidTopic());
}
