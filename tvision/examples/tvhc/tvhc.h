/*-----------------------------------------------------*/
/*                                                     */
/*   Turbo Vision 1.0                                  */
/*   Turbo Vision TVHC header file                     */
/*   Copyright (c) 1991 by Borland International       */
/*                                                     */
/*-----------------------------------------------------*/

#if !defined( TVHC_H )
#define TVHC_H

const int MAXSIZE = 80;
const int MAXSTRSIZE=256;
const char commandChar[] = ".";
const int bufferSize = 4096;

typedef enum State { undefined, wrapping, notWrapping } _State;

class TProtectedStream : public CLY_std(fstream)
{
public:
    TProtectedStream( char *aFileName, CLY_OpenModeT aMode );

private:
    char  fileName[MAXSIZE];
    CLY_OpenModeT mode;
};

// Topic Reference

struct TFixUp
{
    long pos;
    TFixUp *next;
};

union Content
{
    int /*ushort*/ value;
    TFixUp *fixUpList;
};

struct TReference 
{
    char *topic;
    Boolean resolved;
    Content val;
};

class TRefTable : public TSortedCollection
{

public:
    TRefTable( ccIndex aLimit, ccIndex aDelta );
    virtual int compare( void *key1,void *key2 );
    virtual void freeItem( void *item );
    TReference *getReference( char *topic );
    virtual void *keyOf( void *item );

private:
    virtual void *readItem( ipstream& ) { return 0; };
    virtual void writeItem( void *, opstream& ) {};
};

struct TCrossRefNode
{
    char *topic;
    int offset;
    uchar length;
    TCrossRefNode *next;
};

class TTopicDefinition : public TObject
{
public:

    TTopicDefinition(char *aTopic, ushort aValue);
    ~TTopicDefinition(void);

    char *topic;
    ushort value;
    TTopicDefinition *next;

};

char helpName[MAXSIZE];
uchar buffer[bufferSize];
int ofs;
TRefTable *refTable = 0;
TCrossRefNode  *xRefs;
char line[MAXSTRSIZE] = "";
Boolean lineInBuffer = False;
int lineCount = 0;

#endif  // TVHC_H
