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

#include "InProcTestNode.h"

#include <future>

#include <Common/StringTools.h>
#include <Logging/ConsoleLogger.h>

#include "CryptoNoteCore/Core.h"
#include "CryptoNoteCore/MemoryBlockchainCacheFactory.h"
#include "CryptoNoteCore/Miner.h"
#include "CryptoNoteProtocol/CryptoNoteProtocolHandler.h"
#include "P2p/NetNode.h"
#include "InProcessNode/InProcessNode.h"
#include <../tests/Common/VectorMainChainStorage.h>

using namespace CryptoNote;

#undef ERROR

namespace Tests {

namespace {
bool parse_peer_from_string(NetworkAddress &pe, const std::string &node_addr) {
  return ::Common::parseIpAddressAndPort(pe.ip, pe.port, node_addr);
}
}


InProcTestNode::InProcTestNode(const TestNodeConfiguration& cfg, const CryptoNote::Currency& currency, System::Dispatcher& d) : 
  m_cfg(cfg), m_currency(currency), dispatcher(d) {

  std::promise<std::string> initPromise;
  std::future<std::string> initFuture = initPromise.get_future();

  m_thread = std::thread(std::bind(&InProcTestNode::workerThread, this, std::ref(initPromise)));
  auto initError = initFuture.get();

  if (!initError.empty()) {
    m_thread.join();
    throw std::runtime_error(initError);
  }
}

InProcTestNode::~InProcTestNode() {
  if (m_thread.joinable()) {
    m_thread.join();
  }
}

void InProcTestNode::workerThread(std::promise<std::string>& initPromise) {
  System::Dispatcher dispatcher;
  Logging::ConsoleLogger log;
  Logging::LoggerRef logger(log, "InProcTestNode");

  try {
    core.reset(new CryptoNote::Core(
      m_currency,
      log,
      CryptoNote::Checkpoints(log),
      dispatcher,
      std::unique_ptr<IBlockchainCacheFactory>(new MemoryBlockchainCacheFactory("", logger.getLogger())),
      CryptoNote::createVectorMainChainStorage(m_currency)));

    protocol.reset(new CryptoNote::CryptoNoteProtocolHandler(m_currency, dispatcher, *core, NULL, log));
    p2pNode.reset(new CryptoNote::NodeServer(dispatcher, *protocol, log));
    protocol->set_p2p_endpoint(p2pNode.get());

    CryptoNote::NetNodeConfig p2pConfig;

    p2pConfig.setBindIp("127.0.0.1");
    p2pConfig.setBindPort(m_cfg.p2pPort);
    p2pConfig.setExternalPort(0);
    p2pConfig.setAllowLocalIp(false);
    p2pConfig.setHideMyPort(false);
    p2pConfig.setConfigFolder(m_cfg.dataDir);

    std::vector<NetworkAddress> exclusiveNodes;
    for (const auto& en : m_cfg.exclusiveNodes) {
      NetworkAddress na;
      parse_peer_from_string(na, en);
      exclusiveNodes.push_back(na);
    }

    p2pConfig.setExclusiveNodes(exclusiveNodes);

    if (!p2pNode->init(p2pConfig)) {
      throw std::runtime_error("Failed to init p2pNode");
    }

    initPromise.set_value(std::string());

  } catch (std::exception& e) {
    logger(Logging::ERROR) << "Failed to initialize: " << e.what();
    initPromise.set_value(e.what());
    return;
  }

  try {
    p2pNode->run();
  } catch (std::exception& e) {
    logger(Logging::ERROR) << "exception in p2p::run: " << e.what();
  }

  p2pNode->deinit();
  protocol->set_p2p_endpoint(NULL);

  p2pNode.reset();
  protocol.reset();
  core.reset();
}

bool InProcTestNode::startMining(size_t threadsCount, const std::string &address) {
  assert(core.get());
  AccountPublicAddress addr;
  m_currency.parseAccountAddressString(address, addr);
  //return core->startMining(addr, threadsCount);
  assert(false);
  return false;
}

bool InProcTestNode::stopMining() {
  assert(core.get());
  assert(false);
  return false;
}

bool InProcTestNode::stopDaemon() {
  if (!p2pNode.get()) {
    return false;
  }

  p2pNode->sendStopSignal();
  m_thread.join();
  return true;
}

bool InProcTestNode::getBlockTemplate(const std::string &minerAddress, CryptoNote::BlockTemplate &blockTemplate, uint64_t &difficulty) {
  AccountPublicAddress addr;
  BinaryArray extraNonce;
  m_currency.parseAccountAddressString(minerAddress, addr);
  uint32_t height = 0;
  return core->getBlockTemplate(blockTemplate, addr, extraNonce, difficulty, height);
}

bool InProcTestNode::submitBlock(const std::string& block) {
  BinaryArray arr;
  std::copy(block.begin(), block.end(), std::back_inserter(arr));
  return core->submitBlock(std::move(arr)) == std::error_code{};
}

bool InProcTestNode::getTailBlockId(Crypto::Hash &tailBlockId) {
  tailBlockId = core->getTopBlockHash();
  return true;
}

bool InProcTestNode::makeINode(std::unique_ptr<CryptoNote::INode> &node) {

  std::unique_ptr<InProcessNode> inprocNode(new CryptoNote::InProcessNode(*core, *protocol, dispatcher));

  std::promise<std::error_code> p;
  auto future = p.get_future();

  inprocNode->init([&p](std::error_code ec) {
    std::promise<std::error_code> localPromise(std::move(p));
    localPromise.set_value(ec);
  });

  auto ec = future.get();

  if (!ec) {
    node = std::move(inprocNode);
    return true;
  }

  return false;
}

uint64_t InProcTestNode::getLocalHeight() {
  return core->getTopBlockIndex() + 1;
}

}