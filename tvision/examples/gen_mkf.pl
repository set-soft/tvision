#!/usr/bin/perl

for (@ARGV)
   {
    $_=~/([^\/]*)\.mkf/;
    $make=$1;
    $c=scalar(split(/\//));
    $d='';
    while ($c--) { $d.='../'; }
    print "$_ $1 $d\n";
    open(FIL,">$_");
    print FIL "#!/usr/bin/make
#
# Copyright (c) 2003-2004 by Salvador E. Tropea.
# Covered by the GPL license.
#
vpath_src=
vpath_obj=.
INCLUDE_DIRS=".$d."include
COMMON_IMK=".$d."makes/common.imk
include \$(COMMON_IMK)
include $1\$(ExIMK)
";
    close(FIL);
   }





-------------------------------------------------------
This SF.Net email is sponsored by: YOU BE THE JUDGE. Be one of 170
Project Admins to receive an Apple iPod Mini FREE for your judgement on
who ports your project to Linux PPC the best. Sponsored by IBM.
Deadline: Sept. 24. Go here: http://sf.net/ppc_contest.php
_______________________________________________
Tvision-checkins mailing list
Tvision-checkins@lists.sourceforge.net
https://lists.sourceforge.net/lists/listinfo/tvision-checkins


--------------080702000105030906040300
Content-Type: message/rfc822;
 name="CVS: tvision/examples/progba test.mkf,1.1.2.1,1.1.2.2"
Content-Transfer-Encoding: 7bit
Content-Disposition: inline;
 filename="CVS: tvision/examples/progba test.mkf,1.1.2.1,1.1.2.2"

Return-path: <tvision-checkins-admin@lists.sourceforge.net>
Envelope-to: salvador@inti.gov.ar
Delivery-date: Tue, 21 Sep 2004 17:29:16 -0300
Received: from liam.inti.gov.ar ([200.10.161.17])
	by violeta.inti.gov.ar with smtp (Exim 4.40)
	id 1C9rGG-0002R4-Ru
	for salvador@inti.gov.ar; Tue, 21 Sep 2004 17:29:16 -0300
Received: from sc8-sf-mx2.sourceforge.net ([66.35.250.206])
 by liam.inti.gov.ar (SAVSMTP 3.1.0.29) with SMTP id M2004092117291529873
 for <salvador@inti.gov.ar>; Tue, 21 Sep 2004 17:29:16 -0300
Received: from sc8-sf-list2-b.sourceforge.net ([10.3.1.8] helo=sc8-sf-list2.sourceforge.net)
	by sc8-sf-mx2.sourceforge.net with esmtp (TLSv1:AES256-SHA:256)
	(Exim 4.41)
	id 1C9rGD-000839-WC
	for set@users.sourceforge.net; Tue, 21 Sep 2004 13:29:14 -0700
Received: from localhost ([127.0.0.1] helo=projects.sourceforge.net)
	by sc8-sf-list2.sourceforge.net with esmtp (Exim 4.30)
	id 1C9rGD-0002iH-Jd
	for set@users.sourceforge.net; Tue, 21 Sep 2004 13:29:13 -0700
Received: from sc8-sf-mx2-b.sourceforge.net ([10.3.1.12] helo=sc8-sf-mx2.sourceforge.net)
	by sc8-sf-list2.sourceforge.net with esmtp (Exim 4.30)
	id 1C9rFu-0002cY-Gn
	for tvision-checkins@lists.sourceforge.net; Tue, 21 Sep 2004 13:28:54 -0700
Received: from sc8-pr-cvs1-f.sourceforge.net ([10.4.1.7] helo=sc8-pr-cvs1.sourceforge.net)
	by sc8-sf-mx2.sourceforge.net with esmtp (TLSv1:AES256-SHA:256)
	(Exim 4.41)
	id 1C9rFu-0007yz-35
	for tvision-checkins@lists.sourceforge.net; Tue, 21 Sep 2004 13:28:54 -0700
Received: from set by sc8-pr-cvs1.sourceforge.net with local (Exim 4.20)
	id 1C9rFo-0004f3-Od
	for tvision-checkins@lists.sourceforge.net; Tue, 21 Sep 2004 13:28:48 -0700
To: tvision-checkins@lists.sourceforge.net
Subject: CVS: tvision/examples/progba test.mkf,1.1.2.1,1.1.2.2
Message-Id: <E1C9rFo-0004f3-Od@sc8-pr-cvs1.sourceforge.net>
From: Salvador Eduardo Tropea <set@users.sourceforge.net>
X-Spam-Score: 0.1 (/)
X-Spam-Report: Spam Filtering performed by sourceforge.net.
	See http://spamassassin.org/tag/ for more details.
	Report problems to http://sf.net/tracker/?func=add&group_id=1&atid=200001
	0.0 SF_CHICKENPOX_SLASH    BODY: Text interparsed with /
	0.0 SF_CHICKENPOX_MINUS    BODY: Text interparsed with -
	0.0 SF_CHICKENPOX_PERIOD   BODY: Text interparsed with .
	0.0 SF_CHICKENPOX_COMMA    BODY: Text interparsed with ,
	0.0 SF_CHICKENPOX_UNDERSCORE BODY: Text interparsed with _
Sender: tvision-checkins-admin@lists.sourceforge.net
Errors-To: tvision-checkins-admin@lists.sourceforge.net
X-BeenThere: tvision-checkins@lists.sourceforge.net
X-Mailman-Version: 2.0.9-sf.net
Precedence: bulk
List-Unsubscribe: <https://lists.sourceforge.net/lists/listinfo/tvision-checkins>,
	<mailto:tvision-checkins-request@lists.sourceforge.net?subject=unsubscribe>
List-Id: <tvision-checkins.lists.sourceforge.net>
List-Post: <mailto:tvision-checkins@lists.sourceforge.net>
List-Help: <mailto:tvision-checkins-request@lists.sourceforge.net?subject=help>
List-Subscribe: <https://lists.sourceforge.net/lists/listinfo/tvision-checkins>,
	<mailto:tvision-checkins-request@lists.sourceforge.net?subject=subscribe>
List-Archive: <http://sourceforge.net/mailarchive/forum.php?forum=tvision-checkins>
Date: Tue, 21 Sep 2004 13:28:48 -0700
X-Spam-Score: 0.1 (/)
X-Spam-Report: Spam Filtering performed by sourceforge.net.
	See http://spamassassin.org/tag/ for more details.
	Report problems to http://sf.net/tracker/?func=add&group_id=1&atid=200001
	0.0 SF_CHICKENPOX_SLASH    BODY: Text interparsed with /
	0.0 SF_CHICKENPOX_MINUS    BODY: Text interparsed with -
	0.0 SF_CHICKENPOX_AT       BODY: Text interparsed with @
	0.0 SF_CHICKENPOX_PERIOD   BODY: Text interparsed with .
	0.0 SF_CHICKENPOX_COMMA    BODY: Text interparsed with ,
	0.0 SF_CHICKENPOX_UNDERSCORE BODY: Text interparsed with _

Update of /cvsroot/tvision/tvision/examples/progba
In directory sc8-pr-cvs1.sourceforge.net:/tmp/cvs-serv17648/examples/progba

Modified Files:
      Tag: r2_0_1u
	test.mkf 
Log Message:
* Fixed: [Compilation] Problems with the examples when using
MAINTAINER_MODE.


