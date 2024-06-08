#define SCFG_TYPES int32_t, int64_t, uint32_t, uint64_t, float_t, double_t, std::string

#include "../scfg.hpp"

int main() {
	scfg::Config config;

	auto& g1 = config.AddGroup("firstGroup");
	auto& g2 = config.AddGroup("secondGoup");

	auto& e1 = g1.AddEntry<int32_t>("e1", 69420);
	auto& e2 = g1.AddEntry<uint64_t>("e2", 6000000);
	auto& e3 = g1.AddEntry<double>("e3", 369.213);
	auto& e4 = g1.AddEntry<float>("e4", 3.141f);
	auto& e5 = g2.AddEntry<std::string>("e5", "69420");

	config.Save("test");
	config.Load("test");
}