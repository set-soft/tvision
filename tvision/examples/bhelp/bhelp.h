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

#if !defined(TBorlandHelpFile_Included)
#define TBorlandHelpFile_Included

class TBorlandHelpTopic: public THelpTopic
{

public:

    TBorlandHelpTopic( fpstream & s, char * comp );
	 short int previous, next;

protected:

    virtual void *read( ipstream& ) { return 0; };

public:

    static const char * const name;
    static TStreamable *build() { return 0; };

};

// TBorlandHelpIndex

class TBorlandHelpIndex : public THelpIndex 
{
public:

    TBorlandHelpIndex( fpstream & );
    TBorlandHelpIndex( StreamableInit ) {};

protected:

    virtual void *read( ipstream& );

public:

    static const char * const name;
    static TStreamable *build();

};

// TBorlandHelpFile

class TBorlandHelpFile : public THelpFile 
{

public:

    TBorlandHelpFile( fpstream & );
    TCollection *search(char *sstr);

    virtual THelpTopic *getTopic( int );
    ushort options, indexscreen;
    uchar height, width, margin;
    ushort glossaryItems;
    char ** glossary;
    char compressiontable[13];

};

#endif // Uses_TBorlandHelpFile && !TBorlandHelpFile_Included

