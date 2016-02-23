#include <maximus/scraper.hpp>

#include <iostream>
#include <deque>
#include <fstream>
#include <functional>
#include <boost/locale.hpp>

#include <supermarx/util/stubborn.hpp>

#include <maximus/parsers/product_parser.hpp>
#include <maximus/parsers/category_parser.hpp>

namespace supermarx
{
	scraper::scraper(product_callback_t _product_callback, tag_hierarchy_callback_t, unsigned int _ratelimit, bool _cache, bool)
	: product_callback(_product_callback)
	, dl("supermarx maximus/1.0", _ratelimit, _cache ? boost::optional<std::string>("./cache") : boost::none)
	, m(dl, [&]() { error_count++; })
	, product_count(0)
	, page_count(0)
	, error_count(0)
	{}

	void scraper::scrape()
	{
		product_count = 0;
		page_count = 0;
		error_count = 0;

		std::function<void(std::string, size_t)> schedule_f([this, &schedule_f](std::string const& curi, size_t i) -> void {
			page_count++;

			std::string puri = curi + "?PageNumber=" + boost::lexical_cast<std::string>(i);
			m.schedule(puri, [=](downloader::response const& response) {
				size_t product_i = 0;
				product_parser pp([&](const message::product_base& p, boost::optional<std::string> const& image_uri, datetime retrieved_on, confidence conf, problems_t probs)
				{
					product_callback(puri, image_uri, p, retrieved_on, conf, probs);

					++product_i;
					++product_count;
				});

				pp.parse(response.body);

				if(product_i > 0 && product_i < 100)
					schedule_f(curi, i+1);
			});
		});

		category_parser cp([&](category_parser::category_uri_t const& _curi)
		{
			static const boost::regex match_category_uri("^(.+)/[^/]*$");
			boost::smatch what;

			if(!boost::regex_match(_curi, what, match_category_uri))
				return;

			std::string curi = what[1];
			schedule_f(curi, 0);
		});

		cp.parse(dl.fetch("http://www.jumbo.com/producten").body);
		m.process_all();
		std::cerr << "Pages: " << page_count << ", products: " << product_count << ", errors: " << error_count << std::endl;
	}

	raw scraper::download_image(const std::string& uri)
	{
		std::string buf(dl.fetch(uri).body);
		return raw(buf.data(), buf.length());
	}
}
