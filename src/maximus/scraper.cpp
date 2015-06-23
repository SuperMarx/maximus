#include <maximus/scraper.hpp>

#include <iostream>
#include <deque>
#include <fstream>
#include <boost/locale.hpp>

#include <supermarx/util/stubborn.hpp>

#include <maximus/parsers/product_parser.hpp>
#include <maximus/parsers/category_parser.hpp>

namespace supermarx
{
	scraper::scraper(callback_t _callback, size_t _ratelimit, bool _cache)
	: callback(_callback)
	, dl("supermarx maximus/1.0", _ratelimit, _cache ? boost::optional<std::string>("./cache") : boost::none)
	{}

	void scraper::scrape()
	{
		const std::string base_uri = "http://www.jumbo.com";

		category_parser cp([&](category_parser::category_uri_t const& _curi)
		{
			static const boost::regex match_category_uri("^(.+)/[^/]*$");
			boost::smatch what;

			if(!boost::regex_match(_curi, what, match_category_uri))
				return;

			std::string curi = what[1];

			for(size_t i = 0; i < 100; ++i)
			{
				std::string puri = curi + "?PageNumber=" + boost::lexical_cast<std::string>(i);

				size_t product_i = 0;
				product_parser pp([&](const message::product_base& p, boost::optional<std::string> const& _image_uri, datetime retrieved_on, confidence conf, problems_t probs)
				{
					boost::optional<std::string> image_uri;
					if(_image_uri)
						image_uri = base_uri + *_image_uri;

					callback(puri, image_uri, p, retrieved_on, conf, probs);

					++product_i;
				});
				pp.parse(dl.fetch(puri));

				if(product_i == 0)
					return;
			}
		});

		cp.parse(dl.fetch(base_uri + "/producten"));
	}

	raw scraper::download_image(const std::string& uri)
	{
		std::string buf(dl.fetch(uri));
		return raw(buf.data(), buf.length());
	}
}
