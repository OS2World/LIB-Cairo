SUBDIRS = pixman test

pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_DATA=pixman-1.pc

$(pkgconfig_DATA): pixman-1.pc.in

snapshot:
	distdir="$(distdir)-`date '+%Y%m%d'`"; \
	test -d "$(srcdir)/.git" && distdir=$$distdir-`cd "$(srcdir)" && git-rev-parse HEAD | cut -c 1-6`; \
	$(MAKE) $(AM_MAKEFLAGS) distdir="$$distdir" dist

USERNAME=$$USER
RELEASE_OR_SNAPSHOT = $$(if test "x$(CAIRO_VERSION_MINOR)" = "x$$(echo "$(CAIRO_VERSION_MINOR)/2*2" | bc)" ; then echo release; else echo snapshot; fi)
RELEASE_CAIRO_HOST =	$(USERNAME)@cairographics.org
RELEASE_CAIRO_DIR =	/srv/cairo.freedesktop.org/www/releases
RELEASE_CAIRO_URL = 	http://cairographics.org/releases
RELEASE_XORG_URL =	http://xorg.freedesktop.org/archive/individual/lib
RELEASE_XORG_HOST =	$(USERNAME)@xorg.freedesktop.org
RELEASE_XORG_DIR =	/srv/xorg.freedesktop.org/archive/individual/lib
RELEASE_ANNOUNCE_LIST = cairo-announce@cairographics.org, xorg-announce@lists.freedesktop.org

tar_gz = $(PACKAGE)-$(VERSION).tar.gz
tar_bz2 = $(PACKAGE)-$(VERSION).tar.bz2

sha1_tgz = $(tar_gz).sha1
md5_tgz = $(tar_gz).md5

sha1_tbz2 = $(tar_bz2).sha1
md5_tbz2 = $(tar_bz2).md5

gpg_file = $(sha1_tgz).asc

$(sha1_tgz): $(tar_gz)
	sha1sum $^ > $@

$(md5_tgz): $(tar_gz)
	md5sum $^ > $@

$(sha1_tbz2): $(tar_bz2)
	sha1sum $^ > $@

$(md5_tbz2): $(tar_bz2)
	md5sum $^ > $@

$(gpg_file): $(sha1_tgz)
	@echo "Please enter your GPG password to sign the checksum."
	gpg --armor --sign $^ 

release-verify-newer:
	@echo -n "Checking that no $(VERSION) release already exists at $(RELEASE_XORG_HOST)..."
	@ssh $(RELEASE_XORG_HOST) test ! -e $(RELEASE_XORG_DIR)/$(tar_gz) \
		|| (echo "Ouch." && echo "Found: $(RELEASE_XORG_HOST):$(RELEASE_XORG_DIR)/$(tar_gz)" \
		&& echo "Refusing to try to generate a new release of the same name." \
		&& false)
	@ssh $(RELEASE_CAIRO_HOST) test ! -e $(RELEASE_CAIRO_DIR)/$(tar_gz) \
		|| (echo "Ouch." && echo "Found: $(RELEASE_CAIRO_HOST):$(RELEASE_CAIRO_DIR)/$(tar_gz)" \
		&& echo "Refusing to try to generate a new release of the same name." \
		&& false)
	@echo "Good."

release-remove-old:
	$(RM) $(tar_gz) $(tar_bz2) $(sha1_tgz) $(gpg_file)

ensure-prev:
	@if [[ "$(PREV)" == "" ]]; then							\
		echo ""							          &&	\
		echo "You must set the PREV variable on the make command line to" &&	\
		echo "the git tag of the last release."				  &&	\
		echo ""								  &&	\
		echo "For example:"						  &&	\
		echo "      make PREV=pixman-0.7.3"				  &&	\
		echo ""								  &&	\
		false;									\
	fi

release-check: ensure-prev release-verify-newer release-remove-old distcheck

release-upload: release-check $(tar_gz) $(tar_bz2) $(sha1_tgz) $(gpg_file)
	mkdir -p releases
	scp $(tar_gz) $(sha1_tgz) $(gpg_file) $(RELEASE_CAIRO_HOST):$(RELEASE_CAIRO_DIR)
	scp $(tar_gz) $(tar_bz2) $(RELEASE_XORG_HOST):$(RELEASE_XORG_DIR)
	ssh $(RELEASE_CAIRO_HOST) "rm -f $(RELEASE_CAIRO_DIR)/LATEST-$(PACKAGE)-[0-9]* && ln -s $(tar_gz) $(RELEASE_CAIRO_DIR)/LATEST-$(PACKAGE)-$(VERSION)"
	git-tag -s -m "$(PACKAGE) $(VERSION) release" $(PACKAGE)-$(VERSION)

release-publish-message: $(sha1_tgz) $(md5_tgz) $(sha1_tbz2) $(md5_tbz2) ensure-prev
	@echo "Please follow the instructions in RELEASING to push stuff out and"
	@echo "send out the announcement mails.  Here is the excerpt you need:"
	@echo ""
	@echo "Lists:  $(RELEASE_ANNOUNCE_LIST)"
	@echo "Subject: [ANNOUNCE] $(PACKAGE) release $(VERSION) now available"
	@echo "============================== CUT HERE =============================="
	@echo "A new $(PACKAGE) release $(VERSION) is now available"
	@echo ""
	@echo "tar.gz:"
	@echo "	$(RELEASE_CAIRO_URL)/$(tar_gz)"
	@echo "	$(RELEASE_XORG_URL)/$(tar_gz)"
	@echo ""
	@echo "tar.bz2:"
	@echo "	$(RELEASE_XORG_URL)/$(tar_bz2)"
	@echo ""
	@echo "Hashes:"
	@echo -n "	MD5:  "
	@cat $(md5_tgz)
	@echo -n "	MD5:  "
	@cat $(md5_tbz2)
	@echo -n "	SHA1: "
	@cat $(sha1_tgz)
	@echo -n "	SHA1: "
	@cat $(sha1_tbz2)
	@echo ""
	@echo "GPG signature:"
	@echo "	$(RELEASE_CAIRO_URL)/$(gpg_file)"
	@echo "	(signed by `getent passwd "$$USER" | cut -d: -f 5 | cut -d, -f 1`)"
	@echo ""
	@echo "Git:"
	@echo "	git://git.freedesktop.org/git/pixman"
	@echo "	tag: $(PACKAGE)-$(VERSION)"
	@echo ""
	@echo "Log:"
	@git-log --no-merges "$(PACKAGE)-$(PREV)".."$(PACKAGE)-$(VERSION)" | git shortlog | awk '{ printf "\t"; print ; }' | cut -b0-80
	@echo "============================== CUT HERE =============================="
	@echo ""

release-publish: release-upload release-publish-message

.PHONY: release-upload release-publish release-publish-message
