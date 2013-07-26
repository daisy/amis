/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://daisy.org/amis

Copyright (c) 2004 DAISY Consortium

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include "AmisCore.h"
#include "io/UrlInputSource.h"
#include "ambulant/net/datasource.h"
#include "ambulant/common/factory.h"
#include "ambulant/common/plugin_engine.h"
#ifdef AMIS_PLATFORM_WINDOWS
#include "ambulant/net/win32_datasource.h"
#else
#include "ambulant/net/posix_datasource.h"
#endif


ambulant::net::datasource_factory *amis::io::UrlInputSource::s_df = NULL;

amis::io::UrlInputSource* amis::io::NewUrlInputSource(const ambulant::net::url& url)
{
	XMLByte *buf;
	size_t size;
	if (UrlInputSource::s_df == NULL) {
		// None set yet. Create one, for the time being.
		// XXXJack: this is really an omission in the Ambulant API. It needs to get
		// an "application" object that holds all this info.
		UrlInputSource::s_df = new ambulant::net::datasource_factory();
		#if defined(AMIS_PLATFORM_WINDOWS)
		UrlInputSource::s_df->add_raw_factory(ambulant::net::get_win32_datasource_factory());
		#else
		//gives a linker error: where are the ambulant posix datasources now?
		//UrlInputSource::s_df->add_raw_factory(ambulant::net::create_posix_datasource_factory());
		#endif

		ambulant::common::factories *fact = new ambulant::common::factories();
		fact->init_factories();
		fact->set_datasource_factory(UrlInputSource::s_df);
		ambulant::common::plugin_engine *pe = ambulant::common::plugin_engine::get_plugin_engine();
		assert(pe);
		pe->add_plugins(fact);
		// XXXX NOTE: we are leaking fact on purpose: otherwise s_df will be destroyed...
	}
	bool ok = ambulant::net::read_data_from_url(url, UrlInputSource::s_df, (char **)&buf, &size);
	if (!ok) return NULL;
	return new UrlInputSource(buf, size);
}

amis::io::UrlInputSource::UrlInputSource(XMLByte *buf, size_t size)
:	MemBufInputSource(buf, size, "UrlInputSource"),
	m_buf(buf),
	m_size(size)
{
}

amis::io::UrlInputSource::~UrlInputSource()
{
	free((void*)m_buf);
}

void amis::io::UrlInputSource::SetDatasourceFactory(ambulant::net::datasource_factory *df)
{
	s_df = df;
}
