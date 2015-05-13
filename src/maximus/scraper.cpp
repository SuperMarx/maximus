#include <maximus/scraper.hpp>

#include <iostream>
#include <deque>
#include <fstream>
#include <boost/locale.hpp>

#include <supermarx/util/stubborn.hpp>

namespace supermarx
{
	scraper::scraper(callback_t _callback, size_t _ratelimit)
	: callback(_callback)
	, dl("supermarx maximus/1.0", _ratelimit)
	{}

	void scraper::scrape()
	{
	}

	raw scraper::download_image(const std::string& uri)
	{
		std::string buf(dl.fetch(uri));
		return raw(buf.data(), buf.length());
	}
}
