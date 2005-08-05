ES_FILE=es.mo

LANGUAGES=es ru pl
LANG_FILES=$(addsuffix .mo,$(LANGUAGES))

all: $(LANG_FILES)

ifeq ($(msgfmt),)
msgfmt=msgfmt
endif

ifeq ($(xgettext),)
xgettext=xgettext
endif

destmo=tvision$(shell cat ../version.txt).mo

clean:
	rm -f $(addsuffix .mo,$(LANGUAGES))

%.mo: %.po
	-$(msgfmt) -o $@ $<

languages: $(addsuffix .mo,$(LANGUAGES))

install: $(LANG_FILES)
	install -d -m 0755 $(prefix)/share/locale/es/LC_MESSAGES
	install -m 0644 $(ES_FILE) $(prefix)/share/locale/es/LC_MESSAGES/$(destmo)
	install -d -m 0755 $(prefix)/share/locale/ru/LC_MESSAGES
	install -m 0644 ru.mo $(prefix)/share/locale/ru/LC_MESSAGES/$(destmo)
	install -d -m 0755 $(prefix)/share/locale/pl/LC_MESSAGES
	install -m 0644 pl.mo $(prefix)/share/locale/pl/LC_MESSAGES/$(destmo)

tvision.po:
	$(xgettext) --default-domain=dummy --add-comments --keyword=_ \
	  --keyword=__ --directory=$(srcdir) --omit-header \
	  --add-location ../classes/*.cc ../classes/*/*.cc ../*.cc
	touch dummy.po
	cp dummy.po $@
	rm dummy.po

