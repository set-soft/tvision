/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 * Modified by Salvador E. Tropea. Covered by the GPL license.
 * * Change NO_STREAM usage.
 * * Fixed the code to avoid buffer overflows.
 * * Modified to use TVCodePage.
 */

#define Uses_TPXPictureValidator
#define Uses_MsgBox
#define Uses_AllocLocal
#define Uses_TVCodePage
#include <tv.h>

// TPXPictureValidator

const char * TPXPictureValidator::errorMsg = __("Error in picture format.\n %s");

TPXPictureValidator::TPXPictureValidator(const char* aPic, Boolean autoFill)
    : TValidator()
{
  char s[1];

  pic = newStr(aPic);
  if ( autoFill )
       options |= voFill;
  s[0] = 0;
  if (picture(s, False) != prEmpty)
    status = vsSyntax;
}

#if !defined( NO_STREAM )

TPXPictureValidator::TPXPictureValidator( StreamableInit s ) : TValidator(s)
{
}

TStreamable * TPXPictureValidator::build()
{
  return (TStreamable *) new TPXPictureValidator(streamableInit);
}

void TPXPictureValidator::write( opstream& os )
{
  TValidator::write(os);
  os.writeString(pic);
}

void* TPXPictureValidator::read( ipstream& is )
{
  TValidator::read(is);
  pic = is.readString();
  index = jndex = 0;

  return this;
}

#endif // !defined( NO_STREAM )

TPXPictureValidator::~TPXPictureValidator()
{
  delete pic;
};

void TPXPictureValidator::error()
{
  messageBox(mfError | mfOKButton, errorMsg, pic);
}

Boolean TPXPictureValidator::isValidInput(char* s, Boolean suppressFill)
{
  Boolean doFill = Boolean(((options&voFill)!=0) && !suppressFill);

  return Boolean((pic==0) || (picture( (char*)s, doFill) != prError));
}

Boolean TPXPictureValidator::isValid(const char* s)
{
  // SET: Modified to allocate memory and avoid buffer overflows.
  int l = strlen(s) + 1;
  AllocLocalStr(str, l);
  strcpy(str, s);

  return Boolean((pic == 0) || (picture(str, False) == prComplete));
}

static inline
Boolean isSpecial(char ch, const char* special)
{
  return strchr(special, ch) ? True : False;
}

static inline
Boolean isComplete(TPicResult result)
{
  return  Boolean((result == prComplete) || (result == prAmbiguous));
}

static inline
Boolean isIncomplete(TPicResult result)
{
  return Boolean( (result == prIncomplete) || (result == prIncompNoFill) );
}


// TPXPictureValidator members

// Consume input
void TPXPictureValidator::consume(char ch, char* input)
{
      input[jndex] = ch;
      index++;
      jndex++;
}

// Skip a character or a picture group

void TPXPictureValidator::toGroupEnd(int& i, int termCh)
{
      int  brkLevel, brcLevel;

      brkLevel = 0;
      brcLevel = 0;
      do {
        if (i == termCh)
            return;
        else
          switch (pic[i])
          {
          case  '[': brkLevel++; break;
          case  ']': brkLevel--; break;
          case  '{': brcLevel++; break;
          case  '}': brcLevel--; break;
          case  ';': i++; break;
          }
        i++;
      } while (! ((brkLevel == 0) && (brcLevel == 0)));
}

// Find the a comma separator
Boolean TPXPictureValidator::skipToComma(int termCh)
{
      do {
          toGroupEnd(index, termCh); 
      } while (! ( (index == termCh) || (pic[index] == ',')));

      if (pic[index] == ',')
          index++;
      return Boolean(index < termCh);
}

// Calclate the end of a group 
int TPXPictureValidator::calcTerm(int termCh)
{
      int k = index;
      toGroupEnd(k, termCh);
      return k;
}

// The next group is repeated X times }
TPicResult TPXPictureValidator::iteration(char* input, int inTerm)
{
      int itr, k, l;
      TPicResult rslt;
      int termCh;

      itr = 0;
      rslt = prError;

      index++;  // Skip '*'

      // Retrieve number

      while (TVCodePage::isNumber(pic[index]))
      {
        itr = itr * 10 + (pic[index] - '0');
        index++;
      }

      k = index;
      termCh = calcTerm(inTerm);

      // If Itr is 0 allow any number, otherwise enforce the number
      if (itr != 0)
      {
        for (l = 1; l <= itr; l++)
        {
          index = k;
          rslt = process(input,termCh);
          if ( ! isComplete(rslt))
          {
                // Empty means incomplete since all are required
                if (rslt == prEmpty)
                rslt = prIncomplete;
    
            return rslt;
          }
        }
      }
      else
      {
        do {
          index = k;
          rslt = process(input, termCh);
        } while (rslt == prComplete);

        if ((rslt == prEmpty) || (rslt == prError))
        {
          index++;
          rslt = prAmbiguous;
        }
      }
      index = termCh;

      return rslt;
}

// Process a picture group
TPicResult TPXPictureValidator::group(char* input, int inTerm)
{
      TPicResult rslt;
      int termCh;

      termCh = calcTerm(inTerm);
      index++;
      rslt = process(input, termCh - 1);

      if (! isIncomplete(rslt))  
          index = termCh;

      return rslt;
}


TPicResult TPXPictureValidator::checkComplete(TPicResult rslt, int termCh)
{
    int j = index;
    Boolean Status=True;

    if (isIncomplete(rslt))
    {
        // Skip optional pieces
        while (Status)
          switch (pic[j])
          {
          case '[': 
             toGroupEnd(j, termCh);
             break;
          case  '*':
             if (! TVCodePage::isNumber(pic[j + 1]))
                 j++;
             toGroupEnd(j, termCh);
             break;

          default:
              Status = False;
          }

        if (j == termCh)
          rslt = prAmbiguous;
    }

    return rslt;
}


TPicResult TPXPictureValidator::scan(char* input, int termCh)
{
    char ch;
    TPicResult rslt, rScan;

    rScan = prError;
    rslt = prEmpty;

    while ( (index != termCh) && (pic[index] != ','))
    {
        if ((size_t)jndex >= strlen(input))
        return checkComplete(rslt, termCh);

        ch = input[jndex];
        switch (pic[index])
    {
        case  '#': 
        if (! TVCodePage::isNumber(ch)) 
            return prError;
        else 
            consume(ch, input);
        break;
        case  '?': 
        if (! TVCodePage::isAlpha(ch))
            return prError;
        else 
            consume(ch, input);
        break;
        case  '&': 
        if (! TVCodePage::isAlpha(ch))
            return prError;
                else 
            consume(TVCodePage::toUpper(ch), input);
        break;
        case  '!': 
        consume(TVCodePage::toUpper(ch), input);
        break;
        case  '@':
        consume(ch, input);
        break;
        case  '*':
    
              rslt = iteration(input,termCh);
              if (! isComplete(rslt))
                  return rslt;

              if (rslt == prError) 
              rslt = prAmbiguous;
          break;

        case '{':

              rslt = group(input, termCh);
              if (! isComplete(rslt))
              return rslt;

          break;
        case '[':
            
              rslt = group(input, termCh);
              if (isIncomplete(rslt))
              return rslt;
              if (rslt == prError)
              rslt = prAmbiguous;

          break;

        default:

          if (pic[index] == ';')
          index++;
          if (TVCodePage::toUpper(pic[index]) != TVCodePage::toUpper(ch))
            {
            if (ch == ' ')
             ch = pic[index];
            else
             return rScan;
            }

          consume(pic[index], input);
    }

        if (rslt == prAmbiguous)
          rslt = prIncompNoFill;
        else
          rslt = prIncomplete;
    }

      if (rslt == prIncompNoFill)
        return prAmbiguous;
      else
        return prComplete;
}

TPicResult TPXPictureValidator::process(char* input, int termCh)
{

   TPicResult rslt, rProcess;
   Boolean incomp;
   int oldI, oldJ, incompJ=0, incompI=0;

   incomp = False;
   oldI = index;
   oldJ = jndex;
   do {
      rslt = scan(input, termCh);

      // Only accept completes if they make it farther in the input
      //   stream from the last incomplete

      if ( (rslt == prComplete) && incomp && (jndex < incompJ))
      {
        rslt = prIncomplete;
        jndex = incompJ;
      }

      if ((rslt == prError) || (rslt == prIncomplete))
      {
        rProcess = rslt;

        if (! incomp &&  (rslt == prIncomplete) )
        {
          incomp  = True;
          incompI = index;
          incompJ = jndex;
        }
        index = oldI;
        jndex = oldJ;
        if (! skipToComma(termCh))
        {
          if ( incomp )
          {
            rProcess = prIncomplete;
            index = incompI;
            jndex = incompJ;
          }
          return rProcess;
        }
        oldI = index;
      }
   } while (!((rslt != prError) && (rslt != prIncomplete)));

   if ((rslt == prComplete) && incomp)
      return prAmbiguous;
   else
      return rslt;
}

Boolean TPXPictureValidator::syntaxCheck()
{

    int i, len;
    int brkLevel, brcLevel;

    if (!pic || (strlen(pic) == 0))
        return False;

    if (pic[strlen(pic)-1] == ';')
        return False;

    i = 0;
    brkLevel = 0;
    brcLevel = 0;

    len = strlen(pic);
    while (i < len)
    {
      switch (pic[i])
      {
      case '[': brkLevel++; break;
      case ']': brkLevel--; break;
      case '{': brcLevel++; break;
      case '}': brcLevel--; break;
      case ';': i++;        break;
      }
      i++;
    }

    return Boolean( (brkLevel == 0) && (brcLevel == 0) );
}

TPicResult TPXPictureValidator::picture(char* input, Boolean autoFill)
{
  Boolean reprocess;
  TPicResult rslt;

  if (!syntaxCheck())
      return prSyntax;

  if (!input || strlen(input)==0)
       return prEmpty;

  jndex = 0;
  index = 0;

  rslt = process(input, strlen(pic));

  if ((rslt != prError) && ((size_t)jndex < strlen(input)))
    rslt = prError;

  if ((rslt == prIncomplete) && autoFill)
  {
    reprocess = False;

    while (((size_t)index < strlen(pic)) && !isSpecial(pic[index], "#?&!@*{}[]"))
    {
      if (pic[index] == ';')
          index++;
      int end = strlen(input);
      input[end] = pic[index];
      input[end+1] = 0;
      index++;
      reprocess = True;
    }

    jndex = 0;
    index = 0;
    if (reprocess)
      rslt = process(input, strlen(pic));
  }

  if (rslt == prAmbiguous)
    return prComplete;
  else if (rslt == prIncompNoFill)
    return prIncomplete;
  else
    return rslt;
}

