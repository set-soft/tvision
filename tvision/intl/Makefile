ES_FILE=es.mo

LANGUAGES=es ru
LANG_FILES=$(addsuffix .mo,$(LANGUAGES))

all: $(LANG_FILES)

ifeq ($(msgfmt),)
msgfmt=msgfmt
endif

clean:
	rm -f $(addsuffix .mo,$(LANGUAGES))

%.mo: %.po
	-$(msgfmt) -o $@ $<

languages: $(addsuffix .mo,$(LANGUAGES))

install: $(LANG_FILES)
	install -d -m 0755 $(prefix)/share/locale/es/LC_MESSAGES
	install -m 0644 $(ES_FILE) $(prefix)/share/locale/es/LC_MESSAGES/tvision.mo
	install -d -m 0755 $(prefix)/share/locale/ru/LC_MESSAGES
	install -m 0644 ru.mo $(prefix)/share/locale/ru/LC_MESSAGES/tvision.mo

