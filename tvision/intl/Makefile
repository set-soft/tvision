RECODE=recode
RECODE_SEP=..
COPY_RECODE=perl utod.pl

ifneq ($(strip $(DJDIR)),)
export editor_OS:=DJGPP
endif

ifeq ($(strip $(editor_OS)),)
export editor_OS:=UNIX
endif

ifeq ($(editor_OS),DJGPP)
ES_FILE=es.mo
else
ES_FILE=es_iso.mo
endif


LANGUAGES=es ru
LANG_FILES=$(addsuffix .mo,$(LANGUAGES)) es_iso.mo

all: $(LANG_FILES)

es_iso.mo: es_iso.po

es_iso.po: es.po
	$(COPY_RECODE) es.po es_iso.po
	$(RECODE) 850$(RECODE_SEP)iso88591 es_iso.po

ifeq ($(msgfmt),)
msgfmt=msgfmt
endif

clean:
	rm -f $(addsuffix .mo,$(LANGUAGES)) es_iso.*

%.mo: %.po
	-$(msgfmt) -o $@ $<

languages: $(addsuffix .mo,$(LANGUAGES))

install: $(LANG_FILES)
	install -d -m 0755 $(prefix)/share/locale/es/LC_MESSAGES
	install -m 0644 $(ES_FILE) $(prefix)/share/locale/es/LC_MESSAGES/tvision.mo
	install -d -m 0755 $(prefix)/share/locale/ru/LC_MESSAGES
	install -m 0644 ru.mo $(prefix)/share/locale/ru/LC_MESSAGES/tvision.mo

