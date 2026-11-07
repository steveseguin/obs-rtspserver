#include "xop/AACSource.h"

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

static bool check(uint32_t rate, uint32_t channels, const std::string &config)
{
	std::unique_ptr<xop::AACSource> source(
		xop::AACSource::CreateNew(rate, channels, false));
	const std::string expected =
		"a=rtpmap:97 MPEG4-GENERIC/" + std::to_string(rate) + "/" +
		std::to_string(channels) + "\r\n"
		"a=fmtp:97 profile-level-id=1;mode=AAC-hbr;"
		"sizelength=13;indexlength=3;indexdeltalength=3;config=" + config;
	const std::string actual = source->GetAttribute();
	if (actual == expected)
		return true;
	std::cerr << rate << " Hz / " << channels << " channels:\n"
		  << "expected: " << expected << "\nactual:   " << actual << '\n';
	return false;
}

int main()
{
	int failed = 0;
	int cases = 0;
	const auto run = [&](uint32_t rate, uint32_t channels,
			     const std::string &config) {
		++cases;
		if (!check(rate, channels, config))
			++failed;
	};

	// AAC-LC: object type 2, frequency index 3, channel configuration 6.
	// This must remain hexadecimal; atoi("11b0") would produce 11.
	run(48000, 6, "11b0");
	run(48000, 4, "11a0");
	run(48000, 5, "11a8");
	run(48000, 2, "1190");
	run(44100, 6, "1230");
	run(44100, 2, "1210");

	// Exercise every supported standard frequency index after index 0.
	// The existing index-0 rate-table entry is outside this formatting fix.
	const uint32_t rates[] = {88200, 64000, 48000, 44100, 32000, 24000,
				 22050, 16000, 12000, 11025, 8000, 7350};
	for (uint32_t index = 1; index <= 12; ++index) {
		for (uint32_t channels = 1; channels <= 6; ++channels) {
			// ISO/IEC 14496-3 AudioSpecificConfig's 5/4/4-bit fields,
			// followed by the three zero GASpecificConfig flags.
			const uint16_t bits = (2u << 11) | (index << 7) |
					      (channels << 3);
			std::ostringstream hex;
			hex << std::hex << std::setfill('0') << std::setw(4) << bits;
			run(rates[index - 1], channels, hex.str());
		}
	}
	++cases;
	std::unique_ptr<xop::AACSource> unsupported(
		xop::AACSource::CreateNew(12345, 2, false));
	if (!unsupported->GetAttribute().empty()) {
		std::cerr << "Unsupported sample rate should have no attributes\n";
		++failed;
	}

	std::cout << cases - failed << '/' << cases << " cases passed\n";
	return failed ? 1 : 0;
}
