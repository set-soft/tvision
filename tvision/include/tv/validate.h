/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.

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

