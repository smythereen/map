# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
# Copyright (c) 2009-2026 John R. Smythe Jr.
#

#
#	Global Definitions
#

include		../rules.mak

#
#	Unit and Common Header Definitions
#

CHDRS =	$(HDR)/$(UNIT).h

#
#	Unit's Library Definitions
#

# libmap is already linked by the shared product rules.
LIBRARIES =	-lsem	\
		-lsys

#
#	Product Definitions
#


#
#	Global Targets
#

include		../targets.mak

#
#	Unit's Targets
#


#
#	Include	Object Dependency Files
#

include	.depend
