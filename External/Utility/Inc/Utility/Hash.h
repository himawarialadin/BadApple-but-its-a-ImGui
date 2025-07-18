#pragma once
#include <cstdint>
#include <string_view>

namespace Utility
{
	//https://qiita.com/ktokhrtk/items/4ef0dcef76126b7d521d
	namespace murmur3
	{
		constexpr uint32_t to_uint32(char const* key, size_t i = sizeof(uint32_t), uint32_t u32 = 0) { return i ? to_uint32(key, i - 1, (u32 << 8) | key[i - 1]) : u32; }

		constexpr uint32_t murmur3a_5(uint32_t h) { return (h * 5) + 0xe6546b64; }
		constexpr uint32_t murmur3a_4(uint32_t h) { return murmur3a_5((h << 13) | (h >> 19)); }
		constexpr uint32_t murmur3a_3(uint32_t k, uint32_t h) { return murmur3a_4(h ^ k); }
		constexpr uint32_t murmur3a_2(uint32_t k, uint32_t h) { return murmur3a_3(k * 0x1b873593, h); }
		constexpr uint32_t murmur3a_1(uint32_t k, uint32_t h) { return murmur3a_2((k << 15) | (k >> 17), h); }
		constexpr uint32_t murmur3a_0(uint32_t k, uint32_t h) { return murmur3a_1(k * 0xcc9e2d51, h); }
		constexpr uint32_t murmur3a(char const* key, size_t i, uint32_t h = 0) { return i ? murmur3a(key + sizeof(uint32_t), i - 1, murmur3a_0(to_uint32(key), h)) : h; }

		constexpr uint32_t murmur3b_3(uint32_t k, uint32_t h) { return h ^ k; }
		constexpr uint32_t murmur3b_2(uint32_t k, uint32_t h) { return murmur3b_3(k * 0x1b873593, h); }
		constexpr uint32_t murmur3b_1(uint32_t k, uint32_t h) { return murmur3b_2((k << 15) | (k >> 17), h); }
		constexpr uint32_t murmur3b_0(uint32_t k, uint32_t h) { return murmur3b_1(k * 0xcc9e2d51, h); }
		constexpr uint32_t murmur3b(char const* key, size_t i, uint32_t h) { return i ? murmur3b_0(to_uint32(key, i), h) : h; }

		constexpr uint32_t murmur3c_4(uint32_t h) { return h ^ (h >> 16); }
		constexpr uint32_t murmur3c_3(uint32_t h) { return murmur3c_4(h * 0xc2b2ae35); }
		constexpr uint32_t murmur3c_2(uint32_t h) { return murmur3c_3(h ^ (h >> 13)); }
		constexpr uint32_t murmur3c_1(uint32_t h) { return murmur3c_2(h * 0x85ebca6b); }
		constexpr uint32_t murmur3c_0(uint32_t h) { return murmur3c_1(h ^ (h >> 16)); }
		constexpr uint32_t murmur3c(uint32_t h, size_t len) { return murmur3c_0(h ^ static_cast<uint32_t>(len)); }

	}

	//https://github.com/boostorg/container_hash/blob/develop/include/boost/container_hash/detail/hash_mix.hpp ŽQl
	constexpr std::uint32_t HashMix(uint32_t x)
	{
		uint32_t const m1 = 0x21f0aaad;
		uint32_t const m2 = 0x735a2d97;

		x ^= x >> 16;
		x *= m1;
		x ^= x >> 15;
		x *= m2;
		x ^= x >> 15;

		return x;
	}

	constexpr std::uint32_t HashCombine(uint32_t seed, uint32_t v)
	{
		return HashMix(seed + 0x9e3779b9 + v);
	}
	
	constexpr uint32_t Hash32(char const* str, size_t len) 
	{
		return murmur3::murmur3c(murmur3::murmur3b(str + ((len >> 2) * sizeof(uint32_t)), len & 3, murmur3::murmur3a(str, len >> 2)), len); 
	}

	constexpr uint32_t Hash32(uint64_t value)
	{
		char bytes[sizeof(uint64_t)] = {};
		for (size_t i = 0; i < sizeof(uint64_t); i++)
			bytes[i] = static_cast<char>( (value >> (i * 8)) & 0xFF);

		return Hash32(bytes, sizeof(uint64_t));
	}

	constexpr uint32_t Hash32(std::string_view str)
	{
		return Hash32(str.data(), str.size());
	}

	static inline uint32_t Hash32(void* data)
	{
		return Hash32(reinterpret_cast<uint64_t>(data));
	}

}