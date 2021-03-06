/***
	MIT License

	Copyright (c) 2018 NUMERARE

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.

	Parts of this file are originally Copyright (c) 2012-2017 The CryptoNote developers, The Bytecoin developers
***/

#pragma once
#include "Chaingen.h"

struct gen_block_reward : public test_chain_unit_base {
  gen_block_reward();

  bool generate(std::vector<test_event_entry>& events) const;

  bool check_block_verification_context(std::error_code bve, size_t event_idx, const CryptoNote::BlockTemplate& blk);
  bool check_block_verification_context(std::error_code bve, size_t event_idx, const CryptoNote::RawBlock& /*blk*/);

  bool mark_invalid_block(CryptoNote::Core& c, size_t ev_index, const std::vector<test_event_entry>& events);
  bool mark_checked_block(CryptoNote::Core& c, size_t ev_index, const std::vector<test_event_entry>& events);
  bool check_block_rewards(CryptoNote::Core& c, size_t ev_index, const std::vector<test_event_entry>& events);

private:
  //uint64_t ts_start = 1338224400;
  size_t m_invalid_block_index;
  std::vector<size_t> m_checked_blocks_indices;
};
