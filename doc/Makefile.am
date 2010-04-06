PROJECT = mach

EXTRA_DIST = $(PROJECT).rss2 $(PROJECT).ics $(man_MANS)

all-local: feeds

# generate feeds
feeds: $(PROJECT).rss2 $(PROJECT).ics

# don't fail fatally if user does not have RDF
$(PROJECT).rss2: $(top_srcdir)/$(PROJECT).doap
	-moap doap -f $(top_srcdir)/$(PROJECT).doap rss > $(PROJECT).rss2

$(PROJECT).ics: $(top_srcdir)/$(PROJECT).doap
	-moap doap -f $(top_srcdir)/$(PROJECT).doap ical > $(PROJECT).ics
