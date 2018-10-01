#include "ClearRecords.h"
#include "madara/utility/Utility.h"

namespace utility = madara::utility;
typedef madara::knowledge::KnowledgeRecord::Integer Integer;

madara::filters::ClearRecords::ClearRecords() {}

madara::filters::ClearRecords::~ClearRecords() {}

void madara::filters::ClearRecords::filter(knowledge::KnowledgeMap& records,
    const transport::TransportContext&, knowledge::Variables&)
{
  records.clear();
}
