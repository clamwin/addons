//-----------------------------------------------------------------------------
// Name:        nsClamWinServiceModule.h
// Product:     ClamWin Antivirus Thunderbird Extension
//
// Author(s):      budtse [budtse at users dot sourceforge dot net]
//
// Created:     2006/11/07
// Copyright:   Copyright ClamWin Pty Ltd (c) 2005-2006
// Licence:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#include "nsIGenericFactory.h"
#include "nsClamWinService.h"

NS_GENERIC_FACTORY_CONSTRUCTOR(nsClamwinService)

static nsModuleComponentInfo components[] = {{
NSCLAMWINSERVICE_CLASSNAME,
NSCLAMWINSERVICE_CID,
NSCLAMWINSERVICE_CONTRACTID,
nsClamwinServiceConstructor,
}};

NS_IMPL_NSGETMODULE("nsClamwinServiceModule", components)