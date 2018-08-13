/**
 * @file CheckpointStreamer.cpp
 * @author David Kyle <david.kyle@shield.ai>
 *
 * This file contains the CheckpointStream class which streams knowledge updates
 * to serialized temporal knowledge (STK) checkpoints.
 **/

#include <chrono>

#include "CheckpointStreamer.h"

#include "madara/logger/Logger.h"
#include "madara/knowledge/ContextGuard.h"

namespace sc = std::chrono;

namespace madara { namespace knowledge {

void CheckpointStreamer::enqueue(std::string name, KnowledgeRecord record)
{
  ContextGuard context_guard (*context_);

  in_buffer.emplace_back(std::move(name), std::move(record));
}

namespace {
  class CheckpointStreamerLister : public VariablesLister
  {
  public:
    using pair_type = std::pair<std::string, KnowledgeRecord>;
    using vector_type = std::vector<pair_type>;
    using iterator_type = vector_type::const_iterator;

    CheckpointStreamerLister(const vector_type &vec) :
      vec_(&vec) {}

    void start(const CheckpointSettings &settings) override
    {
      (void)settings;
      iter_ = vec_->begin();
    }

    std::pair<const char *, const KnowledgeRecord *> next() override
    {
      std::pair<const char *, const KnowledgeRecord *> ret{nullptr, nullptr};

      if (iter_ == vec_->end()) {
        return ret;
      }

      ret.first = iter_->first.c_str();
      ret.second = &iter_->second;

      ++iter_;

      return ret;
    }

  private:
    const vector_type *vec_;
    iterator_type iter_;
  };
}

void CheckpointStreamer::thread_main(CheckpointStreamer *self)
{
  auto period = sc::microseconds(int64_t(1000000 / self->write_hertz_));
  auto wakeup = sc::steady_clock::now() + period;

  madara_logger_log (self->context_->get_logger(), logger::LOG_MINOR,
    "CheckpointStreamer::thread_main:" \
    " created thread at %f hertz (%d ns period)\n",
    self->write_hertz_,
    sc::duration_cast<sc::nanoseconds>(period).count());

  self->settings_.variables_lister = nullptr;

  while (self->keep_running_.test_and_set()) {
    {
      ContextGuard context_guard (*self->context_);

      using std::swap;
      swap(self->in_buffer, self->out_buffer);

      madara_logger_log (self->context_->get_logger(), logger::LOG_TRACE,
        "CheckpointStreamer::thread_main:" \
        " woke up after %d ns and found %d updates\n",
        sc::duration_cast<sc::nanoseconds>(period).count(),
        self->out_buffer.size());
    }

    if (self->out_buffer.size() > 0) {
      CheckpointStreamerLister lister{self->out_buffer};
      self->settings_.variables_lister = &lister;

      self->context_->save_checkpoint(self->settings_);

      self->settings_.variables_lister = nullptr;

      self->out_buffer.clear();
    }

    std::this_thread::sleep_until(wakeup);
    wakeup += period;
  }
}

CheckpointStreamer::~CheckpointStreamer()
{
  terminate();
}

} } // namespace madara::knowledge
