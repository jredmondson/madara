#include "madara/transport/ndds/NDDS_Transport_Read_Thread.h"
#include "madara/utility/Log_Macros.h"
#include "madara/knowledge/Update_Types.h"
#include "madara/utility/Utility.h"

#include "ace/OS_NS_Thread.h"

#include <iostream>
#include <sstream>

madara::transport::NDDS_Read_Thread::NDDS_Read_Thread (
  const std::string & id,
  madara::knowledge::Thread_Safe_Context & context,
       NDDS_Knowledge_UpdateDataReader * reader)
  : id_ (id), context_ (context),
    barrier_ (2), terminated_ (false), 
    mutex_ (), is_not_ready_ (mutex_), is_ready_ (false),
    data_reader_ (reader)
{
  assignment_symbols_.push_back ("=");
  assignment_symbols_.push_back (";");

  this->activate (THR_NEW_LWP | THR_DETACHED, 1);
  
  MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
    DLINFO "NDDS_Read_Thread::NDDS_Read_Thread:" \
    " read thread started\n"));
}

madara::transport::NDDS_Read_Thread::~NDDS_Read_Thread ()
{
  close ();
}

int
madara::transport::NDDS_Read_Thread::close (void)
{
  terminated_ = true;
  enter_barrier ();

  return 0;
}

void
madara::transport::NDDS_Read_Thread::wait_for_ready (void)
{
  if (!is_ready_)
    is_not_ready_.wait ();
}

int madara::transport::NDDS_Read_Thread::enter_barrier (void)
{ 
  MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
    DLINFO "NDDS_Read_Thread::enter_barrier:" \
    " beginning thread barrier for shut down of read thread\n"));

  barrier_.wait ();
  return 0;
}

void madara::transport::NDDS_Read_Thread::handle_assignment (
  NDDS_Knowledge_Update & data)
{
  if (data.key)
  {
    // if we aren't evaluating a message from ourselves, process it
    std::string key = data.key;
    long long value = data.value;
    int result = 0;

    MADARA_DEBUG (MADARA_LOG_MAJOR_DEBUG_INFO, (LM_DEBUG, 
        DLINFO "NDDS_Read_Thread::handle_assignment:" \
        " waiting to process assignment\n"));

    context_.lock ();
    unsigned long long cur_clock = context_.get_clock (key);
    unsigned long cur_quality = context_.get_quality (key);

    // if the data we are updating had a lower clock value or less quality
    // then that means this update is the latest value. Among
    // other things, this means our solution will work even
    // without FIFO channel transports

    // if the data we are updating had a lower clock value
    // then that means this update is the latest value. Among
    // other things, this means our solution will work even
    // without FIFO channel transports
    result = context_.set_if_unequal (key, value, 
                                      data.quality, data.clock, false);

    context_.unlock ();
    
    // if we actually updated the value
    if (result == 1)
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "NDDS_Read_Thread::handle_assignment:" \
        " received data[%s]=%q from %s.\n", 
        key.c_str (), value, data.originator));
    }
    // if the data was already current
    else if (result == 0)
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
          DLINFO "NDDS_Read_Thread::handle_assignment:" \
          " discarded data[%s]=%q from %s as the value was already set.\n",
          key.c_str (), value, data.originator));
    }
    else if (result == -1)
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "NDDS_Read_Thread::handle_assignment:" \
        " discarded data due to null key.\n"));
    }
    else if (result == -2)
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "NDDS_Read_Thread::handle_assignment:" \
        " discarded data[%s]=%q due to lower quality (%u vs %u).\n",
        key.c_str (), value, cur_quality, data.quality));
    }
    else if (result == -3)
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "NDDS_Read_Thread::handle_assignment:" \
        " discarded data[%s]=%q due to older timestamp (%Q vs %Q).\n",
        key.c_str (), value, cur_clock, data.clock));
    }
  }
}

void madara::transport::NDDS_Read_Thread::handle_multiassignment (
  NDDS_Knowledge_Update & data)
{
  if (data.key)
  {
    std::string key;
    char symbol;
    long long value;
    std::stringstream stream (data.key);

    MADARA_DEBUG (MADARA_LOG_MAJOR_DEBUG_INFO, (LM_DEBUG, 
        DLINFO "NDDS_Read_Thread::multiassignment:" \
        " waiting to process multiassignment\n"));

    context_.lock ();
    
    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "NDDS_Read_Thread::multiassignment:" \
        " processing multiassignment (%s).\n",
          data.key));

    while (!stream.eof ())
    {
      stream >> key >> symbol >> value >> symbol;

      int result = 0;
      unsigned long long cur_clock = context_.get_clock (key);
      unsigned long cur_quality = context_.get_quality (key);

      // if the data we are updating had a lower clock value
      // then that means this update is the latest value. Among
      // other things, this means our solution will work even
      // without FIFO channel transports
      result = context_.set_if_unequal (key, value, 
                                        data.quality, data.clock, false);

      // if we actually updated the value
      if (result == 1)
      {
        MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
          DLINFO "NDDS_Read_Thread::handle_multiassignment:" \
          " received data[%s]=%q from %s.\n",
          key.c_str (), value, data.originator));
      }
      // if the data was already current
      else if (result == 0)
      {
        MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
          DLINFO "NDDS_Read_Thread::handle_multiassignment:" \
          " discarded data[%s]=%q from %s as the value was already set.\n",
          key.c_str (), value, data.originator));
      }
      else if (result == -1)
      {
        MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
          DLINFO "NDDS_Read_Thread::handle_multiassignment:" \
          " discarded data due to null key.\n"));
      }
      else if (result == -2)
      {
        MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
          DLINFO "NDDS_Read_Thread::handle_multiassignment:" \
          " discarded data[%s]=%q due to lower quality (%u vs %u).\n",
          key.c_str (), value, cur_quality, data.quality));
      }
      else if (result == -3)
      {
        MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
          DLINFO "NDDS_Read_Thread::handle_multiassignment:" \
          " discarded data[%s]=%q due to older timestamp (%Q vs %Q).\n",
          key.c_str (), value, cur_clock, data.clock));
      }
    }
    
    context_.unlock ();
  }
}

int
madara::transport::NDDS_Read_Thread::svc (void)
{
  // if we don't check originator for null, we get phantom sends
  // when the program exits.
  MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
    DLINFO "NDDS_Read_Thread::svc:" \
    " entering processing loop.\n"));

  NDDS_Knowledge_UpdateSeq update_data_list;
  DDS_SampleInfoSeq info_seq;
  DDS_ReturnCode_t rc;

  while (false == terminated_.value ())
  {
    rc = data_reader_->take(
            update_data_list, 
            info_seq, 
            DDS_LENGTH_UNLIMITED,
            DDS_ANY_SAMPLE_STATE, 
            DDS_ANY_VIEW_STATE, 
            DDS_ANY_INSTANCE_STATE);

    // data is valid
    if (rc == DDS_RETCODE_OK)
    {
      for (int i = 0; i < update_data_list.length(); ++i)
      {
        if (madara::knowledge::ASSIGNMENT == update_data_list[i].type)
        {
          MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
            DLINFO "NDDS_Read_Thread::svc:" \
            " processing %s=%q from %s with time %Q and quality %u.\n", 
            update_data_list[i].key, update_data_list[i].value, 
            update_data_list[i].originator,
            update_data_list[i].clock, update_data_list[i].quality));

          handle_assignment (update_data_list[i]);
        }
        else if (madara::knowledge::MULTIPLE_ASSIGNMENT == update_data_list[i].type)
        {
          MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
            DLINFO "NDDS_Read_Thread::svc:" \
            " processing multassignment from %s with time %Q and quality %u.\n",
            update_data_list[i].originator,
            update_data_list[i].clock, update_data_list[i].quality));

          handle_multiassignment (update_data_list[i]);
        }
      }
    }
  }

  enter_barrier ();
  return 0;
}
