#
# This file sets the behavior of the txh generator
#
# Lines starting with # or spaces are skiped except in strings or in [Generate]
# Be carefull with [ it delimits sections!
#

[Configuration]
#
# No external program is needed!
# @.html means: Copy the temporal file to xxxxx.html
#
CommandLine=@.html
Name="Direct HTML 3.X format"

[Delimiters]
# Up to 11 characters
SectionStart=/**[txh]**
# Up to 11 characters
SectionEnd=*********/

[Variables]
#
# Up to 16 definitions
#
# Codes for the behavior of the definitions:
# 1 Normal, put the content if found.
# 2 Repeat, use the last value found in the file, ~no is an exeption, ~clear stops
# 3 If not found replace by the prototype.
# 4 If not found replace by the class.
# 5 If not found replace by the name of the function
# 6 It disables the node generation for this comment. The variable is
#   stored in the first variable that have associations and is in the comment.
#
# Additionally there are 2 special variables:
# 90 Name of the file
# 91 Line number of the end of the comment
# 92 Name of the var 0 in the format: ~0 ~~Distinguish{(~Distinguish)~} (node name)
#
# 0 The first variable is the main index variable
#
AddDefinition=Function,5
# 1
AddDefinition=Class,4
# 2
AddDefinition=Include,2
# 3
AddDefinition=Module,2
# 4
AddDefinition=Prototype,3
# 5
AddDefinition=Description,1
# 6
AddDefinition=Return,1
# 7
AddDefinition=Example,1
# 8
AddDefinition=Comments,6
#
# It says what variable is added to distinguish between 2 vars 0 that are equal
#
Distinguish=1

[Associations]
#
# Up to 8 associations
#
# The associations are between the 0 variable and another variable.
#
# Name in main menu, node, variable, optional to add to each node
#
AddAssoc=List by classes,Classes,1,(class)
AddAssoc=List by modules,Modules,3
AddAssoc=List by files,Files,90

[Replace]
#
# All must be delimited by ", they can be used in the GenMain section
#
# Title of the HTML
Constant="Turbo Vision reference"

# Description for the help
Constant="This document describes the functions of Turbo Vision that didn't
exist in the original version of the library.@*
Note that only a few things are documented here, I need help to populate
this file.
@p
This document applies to version 2.0.0
of the Turbo Vision library.
@p"

[Commands]
#
# You can enclose these values between " to make more clear where
# they start and end. Use \n,\r,\t and \" like in C. Use \ at the
# end to concatenate like in C.
#
# @p = end of paragraph
#
EndOfPar="<p>"
#
# @* = break line
#
BreakLine="<br>"
#
# @{value} is the special cross ref.
# ~0 Is the visible name of a reference
# ~1 Is the real name of a reference
#
CrossRef="<a href=\"#~1\">~0</a>"
#
# What we must get from a @@ sequence
#
Double@="@"

[DefinedCommands]
#
# The format is @<name>{parameters ...}
#
var="<b>~0</b>"
subtitle="<Font Size=+1><u>~0</u></Font><p>"
pre=<pre>
/pre=</pre>
link="<a href=\"~0\">~1</a>"
mailto="<a href=\"mailto:~0\">~1</a>"

#
# This section says how to translate ASCIIs
#
[ASCIIConvert]
á=&aacute;
é=&eacute;
í=&iacute;
ó=&oacute;
ú=&uacute;
ñ=&ntilde;
Ñ=&Ntilde;
¡=&iexcl;
¿=&iquest;
ä=&auml;
ë=&euml;
ï=&iuml;
ö=&ouml;
ü=&uuml;
Ü=&Uuml;
à=&agrave;
è=&egrave;
ì=&igrave;
ò=&ograve;
ù=&ugrave;
<=&lt;
>=&gt;

#
#  Use ~number to use one variable
#  Use ~~number{} for conditional, all the code inside {} will become
# conditional
#  This section isn't passed for the macro expansion so here you are
# free to use special commands for the formater.
#
[GenNode]
<p><hr></p>
<a name="~92"></a>
<center><Font Size=+2>~0</Font> (~90 ~91)</center>

<Font Size=+1><u>Syntax</u></Font><p>

~~2{
<pre>
# This line is a comment, but the next is code
 #define Uses_~2
 #include &lt;tv.h&gt;
 
</pre>
~}
~~4{
<pre>
 ~4;
</pre>
~}
~~1{
<Font Size=+1><u>Member of the class:</u></Font>
~1
<p>
~}

~~5{
<Font Size=+1><u>Description</u></Font><p>

~5
<p>
~}
~~6{
<Font Size=+1><u>Return Value</u></Font><p>

~6
<p>
~}
~~7{
<Font Size=+1><u>Example</u></Font><p>

~7
<p>
~}

[GenMenu]
Start="<Menu>"
#
# ~1 is the visible name
# ~2 is the name of the node
#
Entry="<li><a href=\"#~2\">~1</a>"
End="</Menu>"

#
# ~1  is the name of the association
# ~2  is the menu for it
#
[GenAssoMain]
<p><hr></p>
<a name="~1"></a>
<H1>~1</H1>

~2
#
# ~1  is the name of the association
# ~2  is the name without the distinguish
# ~3  is the comment for it
# ~4  is the menu for it
#
[GenAssoRest]
<p><hr></p>
<a name="~1"></a>
<H1>~2</H1>

~3

~4

#
# ~1 Main menu
# ~2 Name of the function list node
# ~3 Menu for all the functions
# ~4 All the associations code
# ~5 All the function nodes
# ~50+ Values from section Replace
#
[GenMain]
<html>
<head>
<title>~50</title>
</head>
<body>
<H1>~50</H1>

~51

~1

<p><hr></p>
<a name="~2"></a>
<H1>~2</H1>

~3

~4

~5

</body>
</html>
