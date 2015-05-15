#include <supermarx/scraper/scraper_cli.hpp>
#include <maximus/scraper.hpp>

int main(int argc, char** argv)
{
	return supermarx::scraper_cli<supermarx::scraper>::exec(3, "maximus", "Jumbo", argc, argv);
}
