///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////

#include "socket_helpers.hpp"

///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef CPPCORO_NET_IP_ENDPOINT_HPP_INCLUDED
#define CPPCORO_NET_IP_ENDPOINT_HPP_INCLUDED

///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef CPPCORO_NET_IP_ADDRESS_HPP_INCLUDED
#define CPPCORO_NET_IP_ADDRESS_HPP_INCLUDED

///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef CPPCORO_NET_IPV4_ADDRESS_HPP_INCLUDED
#define CPPCORO_NET_IPV4_ADDRESS_HPP_INCLUDED

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace cppcoro::net
{
	class ipv4_address
	{
		using bytes_t = std::uint8_t[4];

	public:

		constexpr ipv4_address()
			: m_bytes{ 0, 0, 0, 0 }
		{}

		explicit constexpr ipv4_address(std::uint32_t integer)
			: m_bytes{
			static_cast<std::uint8_t>(integer >> 24),
			static_cast<std::uint8_t>(integer >> 16),
			static_cast<std::uint8_t>(integer >> 8),
			static_cast<std::uint8_t>(integer) }
		{}

		explicit constexpr ipv4_address(const std::uint8_t(&bytes)[4])
			: m_bytes{ bytes[0], bytes[1], bytes[2], bytes[3] }
		{}

		explicit constexpr ipv4_address(
			std::uint8_t b0,
			std::uint8_t b1,
			std::uint8_t b2,
			std::uint8_t b3)
			: m_bytes{ b0, b1, b2, b3 }
		{}

		constexpr const bytes_t& bytes() const { return m_bytes; }

		constexpr std::uint32_t to_integer() const
		{
			return
				std::uint32_t(m_bytes[0]) << 24 |
				std::uint32_t(m_bytes[1]) << 16 |
				std::uint32_t(m_bytes[2]) << 8 |
				std::uint32_t(m_bytes[3]);
		}

		static constexpr ipv4_address loopback()
		{
			return ipv4_address(127, 0, 0, 1);
		}

		constexpr bool is_loopback() const
		{
			return m_bytes[0] == 127;
		}

		constexpr bool is_private_network() const
		{
			return m_bytes[0] == 10 ||
				(m_bytes[0] == 172 && (m_bytes[1] & 0xF0) == 0x10) ||
				(m_bytes[0] == 192 && m_bytes[2] == 168);
		}

		constexpr bool operator==(ipv4_address other) const
		{
			return
				m_bytes[0] == other.m_bytes[0] &&
				m_bytes[1] == other.m_bytes[1] &&
				m_bytes[2] == other.m_bytes[2] &&
				m_bytes[3] == other.m_bytes[3];
		}

		constexpr bool operator!=(ipv4_address other) const
		{
			return !(*this == other);
		}

		constexpr bool operator<(ipv4_address other) const
		{
			return to_integer() < other.to_integer();
		}

		constexpr bool operator>(ipv4_address other) const
		{
			return other < *this;
		}

		constexpr bool operator<=(ipv4_address other) const
		{
			return !(other < *this);
		}

		constexpr bool operator>=(ipv4_address other) const
		{
			return !(*this < other);
		}

		/// Parse a string representation of an IP address.
		///
		/// Parses strings of the form:
		/// - "num.num.num.num" where num is an integer in range [0, 255].
		/// - A single integer value in range [0, 2^32).
		///
		/// \param string
		/// The string to parse.
		/// Must be in ASCII, UTF-8 or Latin-1 encoding.
		///
		/// \return
		/// The IP address if successful, otherwise std::nullopt if the string
		/// could not be parsed as an IPv4 address.
		static std::optional<ipv4_address> from_string(std::string_view string) noexcept;

		/// Convert the IP address to dotted decimal notation.
		///
		/// eg. "12.67.190.23"
		std::string to_string() const;

	private:

		alignas(std::uint32_t) std::uint8_t m_bytes[4];

	};
}

#endif
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef CPPCORO_NET_IPV6_ADDRESS_HPP_INCLUDED
#define CPPCORO_NET_IPV6_ADDRESS_HPP_INCLUDED

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace cppcoro::net
{
	class ipv4_address;

	class ipv6_address
	{
		using bytes_t = std::uint8_t[16];

	public:

		constexpr ipv6_address();

		explicit constexpr ipv6_address(
			std::uint64_t subnetPrefix,
			std::uint64_t interfaceIdentifier);

		constexpr ipv6_address(
			std::uint16_t part0,
			std::uint16_t part1,
			std::uint16_t part2,
			std::uint16_t part3,
			std::uint16_t part4,
			std::uint16_t part5,
			std::uint16_t part6,
			std::uint16_t part7);

		explicit constexpr ipv6_address(
			const std::uint16_t(&parts)[8]);

		explicit constexpr ipv6_address(
			const std::uint8_t(&bytes)[16]);

		constexpr const bytes_t& bytes() const { return m_bytes; }

		constexpr std::uint64_t subnet_prefix() const;

		constexpr std::uint64_t interface_identifier() const;

		/// Get the IPv6 unspedified address :: (all zeroes).
		static constexpr ipv6_address unspecified();

		/// Get the IPv6 loopback address ::1.
		static constexpr ipv6_address loopback();

		/// Parse a string representation of an IPv6 address.
		///
		/// \param string
		/// The string to parse.
		/// Must be in ASCII, UTF-8 or Latin-1 encoding.
		///
		/// \return
		/// The IP address if successful, otherwise std::nullopt if the string
		/// could not be parsed as an IPv4 address.
		static std::optional<ipv6_address> from_string(std::string_view string) noexcept;

		/// Convert the IP address to contracted string form.
		///
		/// Address is broken up into 16-bit parts, with each part represended in 1-4
		/// lower-case hexadecimal with leading zeroes omitted. Parts are separated
		/// by separated by a ':'. The longest contiguous run of zero parts is contracted
		/// to "::".
		///
		/// For example:
		/// ipv6_address::unspecified() -> "::"
		/// ipv6_address::loopback() -> "::1"
		/// ipv6_address(0x0011223344556677, 0x8899aabbccddeeff) ->
		///   "11:2233:4455:6677:8899:aabb:ccdd:eeff"
		/// ipv6_address(0x0102030400000000, 0x003fc447ab991011) ->
		///   "102:304::3f:c447:ab99:1011"
		std::string to_string() const;

		constexpr bool operator==(const ipv6_address& other) const;
		constexpr bool operator!=(const ipv6_address& other) const;
		constexpr bool operator<(const ipv6_address& other) const;
		constexpr bool operator>(const ipv6_address& other) const;
		constexpr bool operator<=(const ipv6_address& other) const;
		constexpr bool operator>=(const ipv6_address& other) const;

	private:

		alignas(std::uint64_t) std::uint8_t m_bytes[16];

	};

	constexpr ipv6_address::ipv6_address()
		: m_bytes{
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0 }
	{}

	constexpr ipv6_address::ipv6_address(
		std::uint64_t subnetPrefix,
		std::uint64_t interfaceIdentifier)
		: m_bytes{
			static_cast<std::uint8_t>(subnetPrefix >> 56),
			static_cast<std::uint8_t>(subnetPrefix >> 48),
			static_cast<std::uint8_t>(subnetPrefix >> 40),
			static_cast<std::uint8_t>(subnetPrefix >> 32),
			static_cast<std::uint8_t>(subnetPrefix >> 24),
			static_cast<std::uint8_t>(subnetPrefix >> 16),
			static_cast<std::uint8_t>(subnetPrefix >> 8),
			static_cast<std::uint8_t>(subnetPrefix),
			static_cast<std::uint8_t>(interfaceIdentifier >> 56),
			static_cast<std::uint8_t>(interfaceIdentifier >> 48),
			static_cast<std::uint8_t>(interfaceIdentifier >> 40),
			static_cast<std::uint8_t>(interfaceIdentifier >> 32),
			static_cast<std::uint8_t>(interfaceIdentifier >> 24),
			static_cast<std::uint8_t>(interfaceIdentifier >> 16),
			static_cast<std::uint8_t>(interfaceIdentifier >> 8),
			static_cast<std::uint8_t>(interfaceIdentifier) }
	{}

	constexpr ipv6_address::ipv6_address(
		std::uint16_t part0,
		std::uint16_t part1,
		std::uint16_t part2,
		std::uint16_t part3,
		std::uint16_t part4,
		std::uint16_t part5,
		std::uint16_t part6,
		std::uint16_t part7)
		: m_bytes{
			static_cast<std::uint8_t>(part0 >> 8),
			static_cast<std::uint8_t>(part0),
			static_cast<std::uint8_t>(part1 >> 8),
			static_cast<std::uint8_t>(part1),
			static_cast<std::uint8_t>(part2 >> 8),
			static_cast<std::uint8_t>(part2),
			static_cast<std::uint8_t>(part3 >> 8),
			static_cast<std::uint8_t>(part3),
			static_cast<std::uint8_t>(part4 >> 8),
			static_cast<std::uint8_t>(part4),
			static_cast<std::uint8_t>(part5 >> 8),
			static_cast<std::uint8_t>(part5),
			static_cast<std::uint8_t>(part6 >> 8),
			static_cast<std::uint8_t>(part6),
			static_cast<std::uint8_t>(part7 >> 8),
			static_cast<std::uint8_t>(part7) }
	{}

	constexpr ipv6_address::ipv6_address(
		const std::uint16_t(&parts)[8])
		: ipv6_address(
			parts[0], parts[1], parts[2], parts[3],
			parts[4], parts[5], parts[6], parts[7])
	{}

	constexpr ipv6_address::ipv6_address(const std::uint8_t(&bytes)[16])
		: m_bytes{
			bytes[0], bytes[1], bytes[2], bytes[3],
			bytes[4], bytes[5], bytes[6], bytes[7],
			bytes[8], bytes[9], bytes[10], bytes[11],
			bytes[12], bytes[13], bytes[14], bytes[15] }
	{}

	constexpr std::uint64_t ipv6_address::subnet_prefix() const
	{
		return
			static_cast<std::uint64_t>(m_bytes[0]) << 56 |
			static_cast<std::uint64_t>(m_bytes[1]) << 48 |
			static_cast<std::uint64_t>(m_bytes[2]) << 40 |
			static_cast<std::uint64_t>(m_bytes[3]) << 32 |
			static_cast<std::uint64_t>(m_bytes[4]) << 24 |
			static_cast<std::uint64_t>(m_bytes[5]) << 16 |
			static_cast<std::uint64_t>(m_bytes[6]) << 8 |
			static_cast<std::uint64_t>(m_bytes[7]);
	}

	constexpr std::uint64_t ipv6_address::interface_identifier() const
	{
		return
			static_cast<std::uint64_t>(m_bytes[8]) << 56 |
			static_cast<std::uint64_t>(m_bytes[9]) << 48 |
			static_cast<std::uint64_t>(m_bytes[10]) << 40 |
			static_cast<std::uint64_t>(m_bytes[11]) << 32 |
			static_cast<std::uint64_t>(m_bytes[12]) << 24 |
			static_cast<std::uint64_t>(m_bytes[13]) << 16 |
			static_cast<std::uint64_t>(m_bytes[14]) << 8 |
			static_cast<std::uint64_t>(m_bytes[15]);
	}

	constexpr ipv6_address ipv6_address::unspecified()
	{
		return ipv6_address{};
	}

	constexpr ipv6_address ipv6_address::loopback()
	{
		return ipv6_address{ 0, 0, 0, 0, 0, 0, 0, 1 };
	}

	constexpr bool ipv6_address::operator==(const ipv6_address& other) const
	{
		for (int i = 0; i < 16; ++i)
		{
			if (m_bytes[i] != other.m_bytes[i]) return false;
		}
		return true;
	}

	constexpr bool ipv6_address::operator!=(const ipv6_address& other) const
	{
		return !(*this == other);
	}

	constexpr bool ipv6_address::operator<(const ipv6_address& other) const
	{
		for (int i = 0; i < 16; ++i)
		{
			if (m_bytes[i] != other.m_bytes[i])
				return m_bytes[i] < other.m_bytes[i];
		}

		return false;
	}

	constexpr bool ipv6_address::operator>(const ipv6_address& other) const
	{
		return (other < *this);
	}

	constexpr bool ipv6_address::operator<=(const ipv6_address& other) const
	{
		return !(other < *this);
	}

	constexpr bool ipv6_address::operator>=(const ipv6_address& other) const
	{
		return !(*this < other);
	}
}

#endif

#include <cassert>
#include <optional>
#include <string>

namespace cppcoro
{
	namespace net
	{
		class ip_address
		{
		public:

			// Constructs to IPv4 address 0.0.0.0
			ip_address() noexcept;

			ip_address(ipv4_address address) noexcept;
			ip_address(ipv6_address address) noexcept;

			bool is_ipv4() const noexcept { return m_family == family::ipv4; }
			bool is_ipv6() const noexcept { return m_family == family::ipv6; }

			const ipv4_address& to_ipv4() const;
			const ipv6_address& to_ipv6() const;

			const std::uint8_t* bytes() const noexcept;

			std::string to_string() const;

			static std::optional<ip_address> from_string(std::string_view string) noexcept;

			bool operator==(const ip_address& rhs) const noexcept;
			bool operator!=(const ip_address& rhs) const noexcept;

			//  ipv4_address sorts less than ipv6_address
			bool operator<(const ip_address& rhs) const noexcept;
			bool operator>(const ip_address& rhs) const noexcept;
			bool operator<=(const ip_address& rhs) const noexcept;
			bool operator>=(const ip_address& rhs) const noexcept;

		private:

			enum class family
			{
				ipv4,
				ipv6
			};

			family m_family;

			union
			{
				ipv4_address m_ipv4;
				ipv6_address m_ipv6;
			};

		};

		inline ip_address::ip_address() noexcept
			: m_family(family::ipv4)
			, m_ipv4()
		{}

		inline ip_address::ip_address(ipv4_address address) noexcept
			: m_family(family::ipv4)
			, m_ipv4(address)
		{}

		inline ip_address::ip_address(ipv6_address address) noexcept
			: m_family(family::ipv6)
			, m_ipv6(address)
		{
		}

		inline const ipv4_address& ip_address::to_ipv4() const
		{
			assert(is_ipv4());
			return m_ipv4;
		}

		inline const ipv6_address& ip_address::to_ipv6() const
		{
			assert(is_ipv6());
			return m_ipv6;
		}

		inline const std::uint8_t* ip_address::bytes() const noexcept
		{
			return is_ipv4() ? m_ipv4.bytes() : m_ipv6.bytes();
		}

		inline bool ip_address::operator==(const ip_address& rhs) const noexcept
		{
			if (is_ipv4())
			{
				return rhs.is_ipv4() && m_ipv4 == rhs.m_ipv4;
			}
			else
			{
				return rhs.is_ipv6() && m_ipv6 == rhs.m_ipv6;
			}
		}

		inline bool ip_address::operator!=(const ip_address& rhs) const noexcept
		{
			return !(*this == rhs);
		}

		inline bool ip_address::operator<(const ip_address& rhs) const noexcept
		{
			if (is_ipv4())
			{
				return !rhs.is_ipv4() || m_ipv4 < rhs.m_ipv4;
			}
			else
			{
				return rhs.is_ipv6() && m_ipv6 < rhs.m_ipv6;
			}
		}

		inline bool ip_address::operator>(const ip_address& rhs) const noexcept
		{
			return rhs < *this;
		}

		inline bool ip_address::operator<=(const ip_address& rhs) const noexcept
		{
			return !(rhs < *this);
		}

		inline bool ip_address::operator>=(const ip_address& rhs) const noexcept
		{
			return !(*this < rhs);
		}
	}
}

#endif
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef CPPCORO_NET_IPV4_ENDPOINT_HPP_INCLUDED
#define CPPCORO_NET_IPV4_ENDPOINT_HPP_INCLUDED

///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef CPPCORO_NET_IPV4_ADDRESS_HPP_INCLUDED
#define CPPCORO_NET_IPV4_ADDRESS_HPP_INCLUDED

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace cppcoro::net
{
	class ipv4_address
	{
		using bytes_t = std::uint8_t[4];

	public:

		constexpr ipv4_address()
			: m_bytes{ 0, 0, 0, 0 }
		{}

		explicit constexpr ipv4_address(std::uint32_t integer)
			: m_bytes{
			static_cast<std::uint8_t>(integer >> 24),
			static_cast<std::uint8_t>(integer >> 16),
			static_cast<std::uint8_t>(integer >> 8),
			static_cast<std::uint8_t>(integer) }
		{}

		explicit constexpr ipv4_address(const std::uint8_t(&bytes)[4])
			: m_bytes{ bytes[0], bytes[1], bytes[2], bytes[3] }
		{}

		explicit constexpr ipv4_address(
			std::uint8_t b0,
			std::uint8_t b1,
			std::uint8_t b2,
			std::uint8_t b3)
			: m_bytes{ b0, b1, b2, b3 }
		{}

		constexpr const bytes_t& bytes() const { return m_bytes; }

		constexpr std::uint32_t to_integer() const
		{
			return
				std::uint32_t(m_bytes[0]) << 24 |
				std::uint32_t(m_bytes[1]) << 16 |
				std::uint32_t(m_bytes[2]) << 8 |
				std::uint32_t(m_bytes[3]);
		}

		static constexpr ipv4_address loopback()
		{
			return ipv4_address(127, 0, 0, 1);
		}

		constexpr bool is_loopback() const
		{
			return m_bytes[0] == 127;
		}

		constexpr bool is_private_network() const
		{
			return m_bytes[0] == 10 ||
				(m_bytes[0] == 172 && (m_bytes[1] & 0xF0) == 0x10) ||
				(m_bytes[0] == 192 && m_bytes[2] == 168);
		}

		constexpr bool operator==(ipv4_address other) const
		{
			return
				m_bytes[0] == other.m_bytes[0] &&
				m_bytes[1] == other.m_bytes[1] &&
				m_bytes[2] == other.m_bytes[2] &&
				m_bytes[3] == other.m_bytes[3];
		}

		constexpr bool operator!=(ipv4_address other) const
		{
			return !(*this == other);
		}

		constexpr bool operator<(ipv4_address other) const
		{
			return to_integer() < other.to_integer();
		}

		constexpr bool operator>(ipv4_address other) const
		{
			return other < *this;
		}

		constexpr bool operator<=(ipv4_address other) const
		{
			return !(other < *this);
		}

		constexpr bool operator>=(ipv4_address other) const
		{
			return !(*this < other);
		}

		/// Parse a string representation of an IP address.
		///
		/// Parses strings of the form:
		/// - "num.num.num.num" where num is an integer in range [0, 255].
		/// - A single integer value in range [0, 2^32).
		///
		/// \param string
		/// The string to parse.
		/// Must be in ASCII, UTF-8 or Latin-1 encoding.
		///
		/// \return
		/// The IP address if successful, otherwise std::nullopt if the string
		/// could not be parsed as an IPv4 address.
		static std::optional<ipv4_address> from_string(std::string_view string) noexcept;

		/// Convert the IP address to dotted decimal notation.
		///
		/// eg. "12.67.190.23"
		std::string to_string() const;

	private:

		alignas(std::uint32_t) std::uint8_t m_bytes[4];

	};
}

#endif

#include <optional>
#include <string>
#include <string_view>

namespace cppcoro
{
	namespace net
	{
		class ipv4_endpoint
		{
		public:

			// Construct to 0.0.0.0:0
			ipv4_endpoint() noexcept
				: m_address()
				, m_port(0)
			{}

			explicit ipv4_endpoint(ipv4_address address, std::uint16_t port = 0) noexcept
				: m_address(address)
				, m_port(port)
			{}

			const ipv4_address& address() const noexcept { return m_address; }

			std::uint16_t port() const noexcept { return m_port; }

			std::string to_string() const;

			static std::optional<ipv4_endpoint> from_string(std::string_view string) noexcept;

		private:

			ipv4_address m_address;
			std::uint16_t m_port;

		};

		inline bool operator==(const ipv4_endpoint& a, const ipv4_endpoint& b)
		{
			return a.address() == b.address() &&
				a.port() == b.port();
		}

		inline bool operator!=(const ipv4_endpoint& a, const ipv4_endpoint& b)
		{
			return !(a == b);
		}

		inline bool operator<(const ipv4_endpoint& a, const ipv4_endpoint& b)
		{
			return a.address() < b.address() ||
				(a.address() == b.address() && a.port() < b.port());
		}

		inline bool operator>(const ipv4_endpoint& a, const ipv4_endpoint& b)
		{
			return b < a;
		}

		inline bool operator<=(const ipv4_endpoint& a, const ipv4_endpoint& b)
		{
			return !(b < a);
		}

		inline bool operator>=(const ipv4_endpoint& a, const ipv4_endpoint& b)
		{
			return !(a < b);
		}
	}
}

#endif
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef CPPCORO_NET_IPV6_ENDPOINT_HPP_INCLUDED
#define CPPCORO_NET_IPV6_ENDPOINT_HPP_INCLUDED

///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef CPPCORO_NET_IPV6_ADDRESS_HPP_INCLUDED
#define CPPCORO_NET_IPV6_ADDRESS_HPP_INCLUDED

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace cppcoro::net
{
	class ipv4_address;

	class ipv6_address
	{
		using bytes_t = std::uint8_t[16];

	public:

		constexpr ipv6_address();

		explicit constexpr ipv6_address(
			std::uint64_t subnetPrefix,
			std::uint64_t interfaceIdentifier);

		constexpr ipv6_address(
			std::uint16_t part0,
			std::uint16_t part1,
			std::uint16_t part2,
			std::uint16_t part3,
			std::uint16_t part4,
			std::uint16_t part5,
			std::uint16_t part6,
			std::uint16_t part7);

		explicit constexpr ipv6_address(
			const std::uint16_t(&parts)[8]);

		explicit constexpr ipv6_address(
			const std::uint8_t(&bytes)[16]);

		constexpr const bytes_t& bytes() const { return m_bytes; }

		constexpr std::uint64_t subnet_prefix() const;

		constexpr std::uint64_t interface_identifier() const;

		/// Get the IPv6 unspedified address :: (all zeroes).
		static constexpr ipv6_address unspecified();

		/// Get the IPv6 loopback address ::1.
		static constexpr ipv6_address loopback();

		/// Parse a string representation of an IPv6 address.
		///
		/// \param string
		/// The string to parse.
		/// Must be in ASCII, UTF-8 or Latin-1 encoding.
		///
		/// \return
		/// The IP address if successful, otherwise std::nullopt if the string
		/// could not be parsed as an IPv4 address.
		static std::optional<ipv6_address> from_string(std::string_view string) noexcept;

		/// Convert the IP address to contracted string form.
		///
		/// Address is broken up into 16-bit parts, with each part represended in 1-4
		/// lower-case hexadecimal with leading zeroes omitted. Parts are separated
		/// by separated by a ':'. The longest contiguous run of zero parts is contracted
		/// to "::".
		///
		/// For example:
		/// ipv6_address::unspecified() -> "::"
		/// ipv6_address::loopback() -> "::1"
		/// ipv6_address(0x0011223344556677, 0x8899aabbccddeeff) ->
		///   "11:2233:4455:6677:8899:aabb:ccdd:eeff"
		/// ipv6_address(0x0102030400000000, 0x003fc447ab991011) ->
		///   "102:304::3f:c447:ab99:1011"
		std::string to_string() const;

		constexpr bool operator==(const ipv6_address& other) const;
		constexpr bool operator!=(const ipv6_address& other) const;
		constexpr bool operator<(const ipv6_address& other) const;
		constexpr bool operator>(const ipv6_address& other) const;
		constexpr bool operator<=(const ipv6_address& other) const;
		constexpr bool operator>=(const ipv6_address& other) const;

	private:

		alignas(std::uint64_t) std::uint8_t m_bytes[16];

	};

	constexpr ipv6_address::ipv6_address()
		: m_bytes{
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0 }
	{}

	constexpr ipv6_address::ipv6_address(
		std::uint64_t subnetPrefix,
		std::uint64_t interfaceIdentifier)
		: m_bytes{
			static_cast<std::uint8_t>(subnetPrefix >> 56),
			static_cast<std::uint8_t>(subnetPrefix >> 48),
			static_cast<std::uint8_t>(subnetPrefix >> 40),
			static_cast<std::uint8_t>(subnetPrefix >> 32),
			static_cast<std::uint8_t>(subnetPrefix >> 24),
			static_cast<std::uint8_t>(subnetPrefix >> 16),
			static_cast<std::uint8_t>(subnetPrefix >> 8),
			static_cast<std::uint8_t>(subnetPrefix),
			static_cast<std::uint8_t>(interfaceIdentifier >> 56),
			static_cast<std::uint8_t>(interfaceIdentifier >> 48),
			static_cast<std::uint8_t>(interfaceIdentifier >> 40),
			static_cast<std::uint8_t>(interfaceIdentifier >> 32),
			static_cast<std::uint8_t>(interfaceIdentifier >> 24),
			static_cast<std::uint8_t>(interfaceIdentifier >> 16),
			static_cast<std::uint8_t>(interfaceIdentifier >> 8),
			static_cast<std::uint8_t>(interfaceIdentifier) }
	{}

	constexpr ipv6_address::ipv6_address(
		std::uint16_t part0,
		std::uint16_t part1,
		std::uint16_t part2,
		std::uint16_t part3,
		std::uint16_t part4,
		std::uint16_t part5,
		std::uint16_t part6,
		std::uint16_t part7)
		: m_bytes{
			static_cast<std::uint8_t>(part0 >> 8),
			static_cast<std::uint8_t>(part0),
			static_cast<std::uint8_t>(part1 >> 8),
			static_cast<std::uint8_t>(part1),
			static_cast<std::uint8_t>(part2 >> 8),
			static_cast<std::uint8_t>(part2),
			static_cast<std::uint8_t>(part3 >> 8),
			static_cast<std::uint8_t>(part3),
			static_cast<std::uint8_t>(part4 >> 8),
			static_cast<std::uint8_t>(part4),
			static_cast<std::uint8_t>(part5 >> 8),
			static_cast<std::uint8_t>(part5),
			static_cast<std::uint8_t>(part6 >> 8),
			static_cast<std::uint8_t>(part6),
			static_cast<std::uint8_t>(part7 >> 8),
			static_cast<std::uint8_t>(part7) }
	{}

	constexpr ipv6_address::ipv6_address(
		const std::uint16_t(&parts)[8])
		: ipv6_address(
			parts[0], parts[1], parts[2], parts[3],
			parts[4], parts[5], parts[6], parts[7])
	{}

	constexpr ipv6_address::ipv6_address(const std::uint8_t(&bytes)[16])
		: m_bytes{
			bytes[0], bytes[1], bytes[2], bytes[3],
			bytes[4], bytes[5], bytes[6], bytes[7],
			bytes[8], bytes[9], bytes[10], bytes[11],
			bytes[12], bytes[13], bytes[14], bytes[15] }
	{}

	constexpr std::uint64_t ipv6_address::subnet_prefix() const
	{
		return
			static_cast<std::uint64_t>(m_bytes[0]) << 56 |
			static_cast<std::uint64_t>(m_bytes[1]) << 48 |
			static_cast<std::uint64_t>(m_bytes[2]) << 40 |
			static_cast<std::uint64_t>(m_bytes[3]) << 32 |
			static_cast<std::uint64_t>(m_bytes[4]) << 24 |
			static_cast<std::uint64_t>(m_bytes[5]) << 16 |
			static_cast<std::uint64_t>(m_bytes[6]) << 8 |
			static_cast<std::uint64_t>(m_bytes[7]);
	}

	constexpr std::uint64_t ipv6_address::interface_identifier() const
	{
		return
			static_cast<std::uint64_t>(m_bytes[8]) << 56 |
			static_cast<std::uint64_t>(m_bytes[9]) << 48 |
			static_cast<std::uint64_t>(m_bytes[10]) << 40 |
			static_cast<std::uint64_t>(m_bytes[11]) << 32 |
			static_cast<std::uint64_t>(m_bytes[12]) << 24 |
			static_cast<std::uint64_t>(m_bytes[13]) << 16 |
			static_cast<std::uint64_t>(m_bytes[14]) << 8 |
			static_cast<std::uint64_t>(m_bytes[15]);
	}

	constexpr ipv6_address ipv6_address::unspecified()
	{
		return ipv6_address{};
	}

	constexpr ipv6_address ipv6_address::loopback()
	{
		return ipv6_address{ 0, 0, 0, 0, 0, 0, 0, 1 };
	}

	constexpr bool ipv6_address::operator==(const ipv6_address& other) const
	{
		for (int i = 0; i < 16; ++i)
		{
			if (m_bytes[i] != other.m_bytes[i]) return false;
		}
		return true;
	}

	constexpr bool ipv6_address::operator!=(const ipv6_address& other) const
	{
		return !(*this == other);
	}

	constexpr bool ipv6_address::operator<(const ipv6_address& other) const
	{
		for (int i = 0; i < 16; ++i)
		{
			if (m_bytes[i] != other.m_bytes[i])
				return m_bytes[i] < other.m_bytes[i];
		}

		return false;
	}

	constexpr bool ipv6_address::operator>(const ipv6_address& other) const
	{
		return (other < *this);
	}

	constexpr bool ipv6_address::operator<=(const ipv6_address& other) const
	{
		return !(other < *this);
	}

	constexpr bool ipv6_address::operator>=(const ipv6_address& other) const
	{
		return !(*this < other);
	}
}

#endif

#include <optional>
#include <string>
#include <string_view>

namespace cppcoro
{
	namespace net
	{
		class ipv6_endpoint
		{
		public:

			// Construct to [::]:0
			ipv6_endpoint() noexcept
				: m_address()
				, m_port(0)
			{}

			explicit ipv6_endpoint(ipv6_address address, std::uint16_t port = 0) noexcept
				: m_address(address)
				, m_port(port)
			{}

			const ipv6_address& address() const noexcept { return m_address; }

			std::uint16_t port() const noexcept { return m_port; }

			std::string to_string() const;

			static std::optional<ipv6_endpoint> from_string(std::string_view string) noexcept;

		private:

			ipv6_address m_address;
			std::uint16_t m_port;

		};

		inline bool operator==(const ipv6_endpoint& a, const ipv6_endpoint& b)
		{
			return a.address() == b.address() &&
				a.port() == b.port();
		}

		inline bool operator!=(const ipv6_endpoint& a, const ipv6_endpoint& b)
		{
			return !(a == b);
		}

		inline bool operator<(const ipv6_endpoint& a, const ipv6_endpoint& b)
		{
			return a.address() < b.address() ||
				(a.address() == b.address() && a.port() < b.port());
		}

		inline bool operator>(const ipv6_endpoint& a, const ipv6_endpoint& b)
		{
			return b < a;
		}

		inline bool operator<=(const ipv6_endpoint& a, const ipv6_endpoint& b)
		{
			return !(b < a);
		}

		inline bool operator>=(const ipv6_endpoint& a, const ipv6_endpoint& b)
		{
			return !(a < b);
		}
	}
}

#endif

#include <cassert>
#include <optional>
#include <string>

namespace cppcoro
{
	namespace net
	{
		class ip_endpoint
		{
		public:

			// Constructs to IPv4 end-point 0.0.0.0:0
			ip_endpoint() noexcept;

			ip_endpoint(ipv4_endpoint endpoint) noexcept;
			ip_endpoint(ipv6_endpoint endpoint) noexcept;

			bool is_ipv4() const noexcept { return m_family == family::ipv4; }
			bool is_ipv6() const noexcept { return m_family == family::ipv6; }

			const ipv4_endpoint& to_ipv4() const;
			const ipv6_endpoint& to_ipv6() const;

			ip_address address() const noexcept;
			std::uint16_t port() const noexcept;

			std::string to_string() const;

			static std::optional<ip_endpoint> from_string(std::string_view string) noexcept;

			bool operator==(const ip_endpoint& rhs) const noexcept;
			bool operator!=(const ip_endpoint& rhs) const noexcept;

			//  ipv4_endpoint sorts less than ipv6_endpoint
			bool operator<(const ip_endpoint& rhs) const noexcept;
			bool operator>(const ip_endpoint& rhs) const noexcept;
			bool operator<=(const ip_endpoint& rhs) const noexcept;
			bool operator>=(const ip_endpoint& rhs) const noexcept;

		private:

			enum class family
			{
				ipv4,
				ipv6
			};

			family m_family;

			union
			{
				ipv4_endpoint m_ipv4;
				ipv6_endpoint m_ipv6;
			};

		};

		inline ip_endpoint::ip_endpoint() noexcept
			: m_family(family::ipv4)
			, m_ipv4()
		{}

		inline ip_endpoint::ip_endpoint(ipv4_endpoint endpoint) noexcept
			: m_family(family::ipv4)
			, m_ipv4(endpoint)
		{}

		inline ip_endpoint::ip_endpoint(ipv6_endpoint endpoint) noexcept
			: m_family(family::ipv6)
			, m_ipv6(endpoint)
		{
		}

		inline const ipv4_endpoint& ip_endpoint::to_ipv4() const
		{
			assert(is_ipv4());
			return m_ipv4;
		}

		inline const ipv6_endpoint& ip_endpoint::to_ipv6() const
		{
			assert(is_ipv6());
			return m_ipv6;
		}

		inline ip_address ip_endpoint::address() const noexcept
		{
			if (is_ipv4())
			{
				return m_ipv4.address();
			}
			else
			{
				return m_ipv6.address();
			}
		}

		inline std::uint16_t ip_endpoint::port() const noexcept
		{
			return is_ipv4() ? m_ipv4.port() : m_ipv6.port();
		}

		inline bool ip_endpoint::operator==(const ip_endpoint& rhs) const noexcept
		{
			if (is_ipv4())
			{
				return rhs.is_ipv4() && m_ipv4 == rhs.m_ipv4;
			}
			else
			{
				return rhs.is_ipv6() && m_ipv6 == rhs.m_ipv6;
			}
		}

		inline bool ip_endpoint::operator!=(const ip_endpoint& rhs) const noexcept
		{
			return !(*this == rhs);
		}

		inline bool ip_endpoint::operator<(const ip_endpoint& rhs) const noexcept
		{
			if (is_ipv4())
			{
				return !rhs.is_ipv4() || m_ipv4 < rhs.m_ipv4;
			}
			else
			{
				return rhs.is_ipv6() && m_ipv6 < rhs.m_ipv6;
			}
		}

		inline bool ip_endpoint::operator>(const ip_endpoint& rhs) const noexcept
		{
			return rhs < *this;
		}

		inline bool ip_endpoint::operator<=(const ip_endpoint& rhs) const noexcept
		{
			return !(rhs < *this);
		}

		inline bool ip_endpoint::operator>=(const ip_endpoint& rhs) const noexcept
		{
			return !(*this < rhs);
		}
	}
}

#endif

#if CPPCORO_OS_WINNT
#include <cstring>
#include <cassert>

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <Windows.h>


cppcoro::net::ip_endpoint
cppcoro::net::detail::sockaddr_to_ip_endpoint(const sockaddr& address) noexcept
{
	if (address.sa_family == AF_INET)
	{
		SOCKADDR_IN ipv4Address;
		std::memcpy(&ipv4Address, &address, sizeof(ipv4Address));

		std::uint8_t addressBytes[4];
		std::memcpy(addressBytes, &ipv4Address.sin_addr, 4);

		return ipv4_endpoint{
			ipv4_address{ addressBytes },
			ntohs(ipv4Address.sin_port)
		};
	}
	else
	{
		assert(address.sa_family == AF_INET6);

		SOCKADDR_IN6 ipv6Address;
		std::memcpy(&ipv6Address, &address, sizeof(ipv6Address));

		return ipv6_endpoint{
			ipv6_address{ ipv6Address.sin6_addr.u.Byte },
			ntohs(ipv6Address.sin6_port)
		};
	}
}

int cppcoro::net::detail::ip_endpoint_to_sockaddr(
	const ip_endpoint& endPoint,
	std::reference_wrapper<sockaddr_storage> address) noexcept
{
	if (endPoint.is_ipv4())
	{
		const auto& ipv4EndPoint = endPoint.to_ipv4();

		SOCKADDR_IN ipv4Address;
		ipv4Address.sin_family = AF_INET;
		std::memcpy(&ipv4Address.sin_addr, ipv4EndPoint.address().bytes(), 4);
		ipv4Address.sin_port = htons(ipv4EndPoint.port());
		std::memset(&ipv4Address.sin_zero, 0, sizeof(ipv4Address.sin_zero));

		std::memcpy(&address.get(), &ipv4Address, sizeof(ipv4Address));

		return sizeof(SOCKADDR_IN);
	}
	else
	{
		const auto& ipv6EndPoint = endPoint.to_ipv6();

		SOCKADDR_IN6 ipv6Address;
		ipv6Address.sin6_family = AF_INET6;
		std::memcpy(&ipv6Address.sin6_addr, ipv6EndPoint.address().bytes(), 16);
		ipv6Address.sin6_port = htons(ipv6EndPoint.port());
		ipv6Address.sin6_flowinfo = 0;
		ipv6Address.sin6_scope_struct = SCOPEID_UNSPECIFIED_INIT;

		std::memcpy(&address.get(), &ipv6Address, sizeof(ipv6Address));

		return sizeof(SOCKADDR_IN6);
	}
}

#endif // CPPCORO_OS_WINNT