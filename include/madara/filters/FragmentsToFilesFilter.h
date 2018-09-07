#ifndef _MADARA_FILTERS_FRAGMENTS_TO_FILES_FILTER_H_
#define _MADARA_FILTERS_FRAGMENTS_TO_FILES_FILTER_H_

#include <string>
#include <vector>
#include "madara/knowledge/KnowledgeUpdateSettings.h"
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/utility/Utility.h"
#include "madara/utility/ScopedArray.h"
#include "madara/knowledge/containers/Vector.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/filters/AggregateFilter.h"

namespace madara
{
  namespace filters
  {
    /**
     * @class FragmentsToFilesFilter
     * @brief Receives fragments and saves them to files
     */
    class FragmentsToFilesFilter : public AggregateFilter
    {
    public:
      /**
       * Constructor
       **/
      FragmentsToFilesFilter ()
      {
      }

      /**
       * Destructor
       **/
      virtual ~FragmentsToFilesFilter () = default;

      /**
       * Filters the fragments and converts them to files
       * @param   records           the aggregate records vector
       **/
      inline virtual void filter (knowledge::KnowledgeMap & records,
        const transport::TransportContext &,
        knowledge::Variables &)
      {
      }
    };
  }
}

#endif // _MADARA_FILTERS_FRAGMENTS_TO_FILES_FILTER_H_
