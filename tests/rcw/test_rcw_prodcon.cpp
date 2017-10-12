#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/rcw/Transaction.h"
#include "madara/knowledge/rcw/Tracked.h"
#include "madara/utility/Utility.h"
#include "madara/threads/Threader.h"
#include "madara/threads/RCWThread.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "test.h"

using namespace madara::knowledge;
using namespace madara::threads;
using namespace madara::knowledge::rcw;

class Producer : public RCWThread
{
  int buf;
  bool ready;
public:
  void setup(Transaction &tx) override
  {
    tx.build("buf", buf).init(0).add();
    ready = false;
    tx.add_init("ready", ready);
  }

  void compute(const Transaction &) override
  {
    if (ready) return;
    ++buf;
    ready = true;
  }
};

class Consumer : public RCWThread
{
  int last_val = 0;
  int buf;
  bool ready;
public:
  void setup(Transaction &tx) override
  {
    tx.build("buf", buf).ro().init(0).add();
    tx.add("ready", ready);
  }

  void compute(const Transaction &) override
  {
    if (!ready) return;
    test_eq(last_val + 1, buf);
    last_val = buf;
    ready = false;
  }
};

int main()
{
  KnowledgeBase kb;
  Transaction tx(kb);

  Producer *prod = new Producer;
  Consumer *cons = new Consumer;

  Threader threader(kb);

  threader.run(10, "producer", prod);
  threader.run(10, "consumer", cons);

  sleep(20);

  tests_finalize();
}
