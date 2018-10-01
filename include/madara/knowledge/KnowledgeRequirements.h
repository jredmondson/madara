#ifndef _MADARA_KNOWLEDGE_KNOWLEDGEREQUIREMENTS_H_
#define _MADARA_KNOWLEDGE_KNOWLEDGEREQUIREMENTS_H_

/**
 * @file KnowledgeRequirements.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the KnowledgeRequirements class, which
 * provides settings for copying knowledge records
 */

#include <string>
#include <map>
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/utility/Utility.h"

namespace madara
{
namespace knowledge
{
/**
 * @class KnowledgeRequirements
 * @brief Holds settings requirements for knowledge, usually in copying
 **/
struct KnowledgeRequirements {
  /**
   * A helper class for matching prefixes and suffixes
   **/
  struct MatchPredicate {
    /// a prefix to match
    std::string prefix;

    /// a suffix to match
    std::string suffix;
  };

  /**
   * Constructor
   **/
  KnowledgeRequirements()
    : clear_knowledge(false),
      clock_threshold(0),
      quality_threshold(0),
      use_source_clock(false)

  {
  }

  /**
   * Constructor
   * @param  t_clear_knowledge   if true, target clears knowledge first
   * @param  t_clock_threshold   the minimum source knowledge clock value
   * @param  t_predicates        a vector of predicates to match
   * @param  t_quality_threshold the minimum source knowledge quality
   * @param  t_use_source_clock  if true, target uses source clock value
   **/
  KnowledgeRequirements(bool t_clear_knowledge, uint64_t t_clock_threshold = 0,
      std::vector<MatchPredicate> t_predicates = {},
      uint64_t t_quality_threshold = 0, bool t_use_source_clock = false)
    : clear_knowledge(t_clear_knowledge),
      clock_threshold(t_clock_threshold),
      predicates(t_predicates),
      quality_threshold(t_quality_threshold),
      use_source_clock(t_use_source_clock)
  {
  }

  /**
   * Copy constructor
   * @param   rhs   settings instance to copy
   **/
  KnowledgeRequirements(const KnowledgeRequirements& rhs)
    : clear_knowledge(rhs.clear_knowledge),
      clock_threshold(rhs.clock_threshold),
      predicates(rhs.predicates),
      quality_threshold(rhs.quality_threshold),
      use_source_clock(rhs.use_source_clock)
  {
  }

  /**
   * If true, during loads, clear the KnowledgeBase first
   **/
  bool clear_knowledge;

  /**
   * Minimum acceptable clock to accept (0 by default)
   **/
  uint64_t clock_threshold;

  /**
   * A vector of acceptable predicates to match (prefix and suffix).
   * Empty elements mean "match all". Consequently, if you have a
   * a predicate with a prefix of "" and a suffix of "", this would
   * simply mean match all records. If you have a prefix of "agent", and
   * an empty suffix, then all variables that start with "agent" will
   * be accepted in the operation. Matching suffixes is more
   * computationally expensive, as it forces all variables to be
   * evaluated. If at all possible, use prefixes. Only use suffixes
   * to filter out prefix results.
   **/
  std::vector<MatchPredicate> predicates;

  /**
   * Minimum acceptable knowledge qualilty to accept (0 by default)
   **/
  uint64_t quality_threshold;

  /**
   * Update to the Lamport Clock of the source
   **/
  bool use_source_clock;
};
}
}
#endif  //_MADARA_KNOWLEDGE_KNOWLEDGEREQUIREMENTS_H_
