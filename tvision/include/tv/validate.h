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

#if defined(Uses_TValidator) && !defined(__TValidator)
#define __TValidator

const ushort	
// TValidator Status constants
		vsOk		= 0,
		vsSyntax	= 1,
// Validator option flags
		voFill		= 0x0001,
		voTransfer	= 0x0002,
		voOnAppend	= 0x0004,
		voReserved	= 0x00F8;

typedef enum {
  vtDataSize,
  vtSetData,
  vtGetData
} TVTransfer;

class TInputLine;

// Abstract TValidator object
class CLY_EXPORT TValidator : public TObject
#if !defined( NO_STREAM )
    , CLY_BROKEN_WATCOM_SCOPE TStreamable
#endif // NO_STREAM
{
public:
  ushort Status;
  ushort Options;
  TValidator();
  virtual void Error() = 0;
  virtual Boolean IsValidInput(char *,Boolean);
  virtual Boolean IsValid(const char *);
  virtual ushort Transfer(char *,void *,TVTransfer);
  virtual void Format(char *source);
  Boolean Valid(const char *);
  void SetOwner(TInputLine *owner) { Owner = owner; }
protected:
  TInputLine *Owner;
#if !defined( NO_STREAM )
private:
  virtual const char *streamableName() const
	{ return name; }
protected:
  TValidator( StreamableInit );
public:
  static const char * const name;
protected:
  virtual void write( opstream& );
  virtual void *read( ipstream& );
#endif // NO_STREAM
};

class TFilterValidator : public TValidator
{
protected:
  char * ValidChars;
public:
  TFilterValidator();
  TFilterValidator(const char *);
  ~TFilterValidator();
  virtual void Error();
  virtual Boolean IsValid(const char *);
  virtual Boolean IsValidInput(char *,Boolean);
#if !defined( NO_STREAM )
private:
  virtual const char *streamableName() const
	{ return name; }
protected:
  TFilterValidator( StreamableInit );
public:
  static const char * const name;
  static TStreamable *build();
protected:
  virtual void write( opstream& );
  virtual void *read( ipstream& );
#endif // NO_STREAM
};

class TRangeValidator : public TFilterValidator
{
  long Min,Max;
public:
  TRangeValidator();
  TRangeValidator(long aMin,long aMax);
  virtual void Error();
  virtual Boolean IsValid(const char *);
  virtual ushort Transfer(char *,void *,TVTransfer);
#if !defined( NO_STREAM )
private:
  virtual const char *streamableName() const
	{ return name; }
protected:
  TRangeValidator( StreamableInit );
public:
  static const char * const name;
  static TStreamable *build();
protected:
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

inline ipstream& operator >> ( ipstream& is, TFilterValidator& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TFilterValidator*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TFilterValidator& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TFilterValidator* cl )
    { return os << (TStreamable *)cl; }

inline ipstream& operator >> ( ipstream& is, TRangeValidator& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TRangeValidator*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TRangeValidator& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TRangeValidator* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif


#if defined(Uses_TPXPictureValidator) && !defined(TPXPictureValidator_Included)
#define TPXPictureValidator_Included

// TPXPictureValidator result type

enum TPicResult {prComplete, prIncomplete, prEmpty, prError, prSyntax,
    prAmbiguous, prIncompNoFill};

// TPXPictureValidator


class TPXPictureValidator : public TValidator
{
    static const char * errorMsg;

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

