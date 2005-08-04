/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador E. Tropea (TPXPictureValidator)

 *
 *
 */

#if defined(Uses_TValidator) && !defined(TValidator_Included)
#define TValidator_Included

const ushort
// TValidator Status constants
                vsOk            = 0,
                vsSyntax        = 1,
// Validator option flags
                voFill          = 0x0001,
                voTransfer      = 0x0002,
                voOnAppend      = 0x0004,
                voReserved      = 0x00F8;

typedef enum {
  vtDataSize,
  vtSetData,
  vtGetData
} TVTransfer;

// TValidator object
class CLY_EXPORT TValidator : public TObject
#if !defined( NO_STREAM )
    , CLY_BROKEN_WATCOM_SCOPE TStreamable
#endif // NO_STREAM
{
public:
  TValidator();
  virtual void error();
  virtual Boolean isValidInput(char *,Boolean);
  virtual Boolean isValid(const char *);
  virtual ushort transfer(char *,void *,TVTransfer);
  virtual void format(char *source);
  Boolean validate(const char *);

  ushort status;
  ushort options;
  
#if !defined( NO_STREAM )
public:
  static const char * const name;
  
private:
  virtual const char *streamableName() const
    { return name; }
    
protected:
  TValidator( StreamableInit );
  virtual void write( opstream& );
  virtual void *read( ipstream& );
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TValidator& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TValidator*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TValidator& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TValidator* cl )
    { return os << (TStreamable *)cl; }
#endif

#endif // defined(Uses_TValidator) && !defined(TValidator_Included)


// TFilterValidator
#if defined(Uses_TFilterValidator) && !defined(TFilterValidator_Included)
#define TFilterValidator_Included

class CLY_EXPORT TFilterValidator : public TValidator
{
public:
  TFilterValidator();
  TFilterValidator(const char *);
  ~TFilterValidator();
  virtual void error();
  virtual Boolean isValid(const char *);
  virtual Boolean isValidInput(char *,Boolean);
  
protected:
  char * validChars;
  
#if !defined( NO_STREAM )
public:
  static const char * const name;
  static TStreamable *build();

private:
  virtual const char *streamableName() const
        { return name; }
        
protected:
  TFilterValidator( StreamableInit );
  virtual void write( opstream& );
  virtual void *read( ipstream& );
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TFilterValidator& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TFilterValidator*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TFilterValidator& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TFilterValidator* cl )
    { return os << (TStreamable *)cl; }
#endif

#endif // defined(Uses_TFilterValidator) && !defined(TFilterValidator_Included)


// TRangeValidator
#if defined(Uses_TRangeValidator) && !defined(TRangeValidator_Included)
#define TRangeValidator_Included

class CLY_EXPORT TRangeValidator : public TFilterValidator
{
public:
  TRangeValidator();
  TRangeValidator(long aMin,long aMax);
  virtual void error();
  virtual Boolean isValid(const char *);
  virtual ushort transfer(char *,void *,TVTransfer);

protected:
  long min, max;

#if !defined( NO_STREAM )
public:
  static const char * const name;
  static TStreamable *build();
        
protected:
  TRangeValidator( StreamableInit );
  virtual void write( opstream& );
  virtual void *read( ipstream& );
  
private:
  virtual const char *streamableName() const
        { return name; }
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TRangeValidator& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TRangeValidator*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TRangeValidator& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TRangeValidator* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif // defined(Uses_TRangeValidator) && !defined(TRangeValidator_Included)


// TPXPictureValidator
#if defined(Uses_TPXPictureValidator) && !defined(TPXPictureValidator_Included)
#define TPXPictureValidator_Included

// TPXPictureValidator result type

enum TPicResult {prComplete, prIncomplete, prEmpty, prError, prSyntax,
    prAmbiguous, prIncompNoFill};

// TPXPictureValidator


class CLY_EXPORT TPXPictureValidator : public TValidator
{
public:
    TPXPictureValidator(const char* aPic, Boolean autoFill);
    ~TPXPictureValidator();
    virtual void error();
    virtual Boolean isValidInput(char* s, Boolean suppressFill);
    virtual Boolean isValid(const char* s);
    virtual TPicResult picture(char* input, Boolean autoFill);

protected:
    char* pic;

private:
    void consume(char ch, char* input);
    void toGroupEnd(int& i, int termCh);
    Boolean skipToComma(int termCh);
    int calcTerm(int);
    TPicResult iteration(char* input, int termCh);
    TPicResult group(char* input, int termCh);
    TPicResult checkComplete(TPicResult rslt, int termCh);
    TPicResult scan(char* input, int termCh);
    TPicResult process(char* input, int termCh);
    Boolean syntaxCheck();

    int index, jndex;
    static const char * errorMsg;

#if !defined( NO_STREAM )
public:
    static TStreamable *build();
    static const char * const name;
    
protected:
    TPXPictureValidator( StreamableInit );
    virtual void write( opstream& os );
    virtual void* read( ipstream& is );

private:
    virtual const char *streamableName() const  {return name;};
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TPXPictureValidator& v )
    { return is >> (TStreamable&)v; }
inline ipstream& operator >> ( ipstream& is, TPXPictureValidator*& v )
    { return is >> (void *&)v; }

inline opstream& operator << ( opstream& os, TPXPictureValidator& v )
    { return os << (TStreamable&)v; }
inline opstream& operator << ( opstream& os, TPXPictureValidator* v )
    { return os << (TStreamable *)v; }
#endif // NO_STREAM

#endif // defined(Uses_TPXPictureValidator) && !defined(TPXPictureValidator_Included)


#if defined(Uses_TLookupValidator) && !defined(TLookupValidator_Included)
#define TLookupValidator_Included

// TLookupValidator

class CLY_EXPORT TLookupValidator : public TValidator
{
public:
    TLookupValidator() : TValidator() {}
    virtual Boolean isValid(const char* s);
    virtual Boolean lookup(const char* s);
    
#if !defined( NO_STREAM )
    static TStreamable *build();
    static const char * const name;
protected:
    TLookupValidator( StreamableInit );
private:
    virtual const char *streamableName() const  {return name;}
#endif
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TLookupValidator& v )
    { return is >> (TStreamable&)v; }
inline ipstream& operator >> ( ipstream& is, TLookupValidator*& v )
    { return is >> (void *&)v; }

inline opstream& operator << ( opstream& os, TLookupValidator& v )
    { return os << (TStreamable&)v; }
inline opstream& operator << ( opstream& os, TLookupValidator* v )
    { return os << (TStreamable *)v; }
#endif

#endif // defined(Uses_TLookupValidator) && !defined(TLookupValidator_Included)


#if defined(Uses_TStringLookupValidator) && !defined(TStringLookupValidator_Included)
#define TStringLookupValidator_Included

// TStringLookupValidator

class TStringCollection;

class CLY_EXPORT TStringLookupValidator : public TLookupValidator
{
public:
    TStringLookupValidator(TStringCollection* aStrings);
    ~TStringLookupValidator();
    virtual void error();
    virtual Boolean lookup(const char* s);
    void newStringList(TStringCollection* aStrings);

protected:
    TStringCollection* strings;
    static const char * errorMsg;

#if !defined( NO_STREAM )
public:
    static TStreamable *build();
    static const char * const name;
    
protected:
    TStringLookupValidator( StreamableInit );
    virtual void write( opstream& os );
    virtual void* read( ipstream& is );

private:
    virtual const char *streamableName() const  {return name;}
#endif
};


#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TStringLookupValidator& v )
    { return is >> (TStreamable&)v; }
inline ipstream& operator >> ( ipstream& is, TStringLookupValidator*& v )
    { return is >> (void *&)v; }

inline opstream& operator << ( opstream& os, TStringLookupValidator& v )
    { return os << (TStreamable&)v; }
inline opstream& operator << ( opstream& os, TStringLookupValidator* v )
    { return os << (TStreamable *)v; }
#endif

#endif // defined(Uses_TStringLookupValidator) && !defined(TStringLookupValidator_Included)

