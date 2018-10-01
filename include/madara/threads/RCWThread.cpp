#include "RCWThread.h"

using namespace madara::threads;
using namespace madara::knowledge;
using namespace madara::knowledge::rcw;

void RCWThread::init(KnowledgeBase& kb)
{
  tx_.reset(new Transaction(kb));
  setup(*tx_);
}

void RCWThread::run(void)
{
  tx_->pull();
  compute(*tx_);
  tx_->push();
}

void RCWThread::cleanup(void)
{
  finalize(*tx_);
}
