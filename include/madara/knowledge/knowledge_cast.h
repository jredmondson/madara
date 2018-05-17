/**
 * Copyright (c) 2015 Carnegie Mellon University. All Rights Reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:

 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following acknowledgments
 * and disclaimers.

 * 2. Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided
 * with the distribution.

 * 3. The names "Carnegie Mellon University," "SEI" and/or "Software
 * Engineering Institute" shall not be used to endorse or promote
 * products derived from this software without prior written
 * permission. For written permission, please contact
 * permission@sei.cmu.edu.

 * 4. Products derived from this software may not be called "SEI" nor
 * may "SEI" appear in their names without prior written permission of
 * permission@sei.cmu.edu.

 * 5. Redistributions of any form whatsoever must retain the following
 * acknowledgment:

 * This material is based upon work funded and supported by the
 * Department of Defense under Contract No. FA8721-05-C-0003 with
 * Carnegie Mellon University for the operation of the Software
 * Engineering Institute, a federally funded research and development
 * center.

 * Any opinions, findings and conclusions or recommendations expressed
 * in this material are those of the author(s) and do not necessarily
 * reflect the views of the United States Department of Defense.

 * NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE
 * ENGINEERING INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS"
 * BASIS. CARNEGIE MELLON UNIVERSITY MAKES NO WARRANTIES OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, AS TO ANY MATTER INCLUDING, BUT NOT
 * LIMITED TO, WARRANTY OF FITNESS FOR PURPOSE OR MERCHANTABILITY,
 * EXCLUSIVITY, OR RESULTS OBTAINED FROM USE OF THE MATERIAL. CARNEGIE
 * MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF ANY KIND WITH
 * RESPECT TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT
 * INFRINGEMENT.

 * This material has been approved for public release and unlimited
 * distribution.

 * DM-0002494
**/

#ifndef INCL_KNOWLEDGE_CAST_HPP
#define INCL_KNOWLEDGE_CAST_HPP

#include <string>
#include <type_traits>
#include <stdbool.h>
#include <madara/knowledge/KnowledgeRecord.h>

namespace madara
{

  namespace knowledge
  {
    template<class T>
    class type {};

    /// By default, call constructor of target class;
    /// for other semantics, define specializations
    template<class O>
    inline auto knowledge_cast(type<O>, const KnowledgeRecord &in) ->
      typename std::enable_if<std::is_constructible<O,
               const KnowledgeRecord &>::value, O>::type
    {
      return O{in};
    }

    template<class O>
    inline auto knowledge_cast(type<O>, const KnowledgeRecord &in) ->
      typename std::enable_if<std::is_floating_point<O>::value, O>::type
    {
      return static_cast<O>(in.to_double());
    }

    template<class O>
    inline auto knowledge_cast(type<O>, const KnowledgeRecord &in) ->
      typename std::enable_if<std::is_integral<O>::value, O>::type
    {
      return static_cast<O>(in.to_integer());
    }

    inline bool knowledge_cast(type<bool>, const KnowledgeRecord &in)
    {
      return in.is_true();
    }

    inline std::string knowledge_cast(type<std::string>, const KnowledgeRecord &in)
    {
      return in.to_string();
    }

    inline std::vector<int64_t> knowledge_cast(type<std::vector<int64_t>>, const KnowledgeRecord &in)
    {
      return in.to_integers();
    }

    inline std::vector<double> knowledge_cast(type<std::vector<double>>, const KnowledgeRecord &in)
    {
      return in.to_doubles();
    }

    inline KnowledgeRecord knowledge_cast(type<KnowledgeRecord>, const KnowledgeRecord &in)
    {
      return in;
    }

    template<typename T>
    inline auto knowledge_cast(const KnowledgeRecord &in) ->
        decltype(knowledge_cast(type<T>{}, in)) {
      return knowledge_cast(type<T>{}, in);
    }

    template<class O>
    inline auto knowledge_cast(O &&in) ->
      typename std::decay<decltype(KnowledgeRecord{std::forward<O>(in)})>::type
    {
      return KnowledgeRecord{std::forward<O>(in)};
    }

    template<typename T>
    inline auto knowledge_cast(const std::vector<T> &in) ->
      typename std::enable_if<
      std::is_integral<T>::value &&
      !std::is_same<T, char>::value &&
      !std::is_same<T, unsigned char>::value &&
      !std::is_same<T, int64_t>::value,
      typename std::decay<decltype(KnowledgeRecord{
          tags::integers, in.begin(), in.end()})>::type>::type
    {
      return KnowledgeRecord{tags::integers, in.begin(), in.end()};
    }

    template<typename T>
    inline auto knowledge_cast(const std::vector<T> &in) ->
      typename std::enable_if<
      std::is_floating_point<T>::value &&
      !std::is_same<T, double>::value,
      typename std::decay<decltype(KnowledgeRecord{
          tags::doubles, in.begin(), in.end()})>::type>::type
    {
      return KnowledgeRecord{tags::doubles, in.begin(), in.end()};
    }

    template<typename T, size_t N>
    inline auto knowledge_cast(const T (&in)[N]) ->
      typename std::enable_if<
      std::is_integral<T>::value &&
      !std::is_same<T, char>::value &&
      !std::is_same<T, unsigned char>::value,
      typename std::decay<decltype(KnowledgeRecord{
          tags::integers, &in[0], &in[N]})>::type>::type
    {
      return KnowledgeRecord{tags::integers, &in[0], &in[N]};
    }

    template<typename T, size_t N>
    inline auto knowledge_cast(const T (&in)[N]) ->
      typename std::enable_if<
      std::is_floating_point<T>::value,
      typename std::decay<decltype(KnowledgeRecord{
          tags::doubles, &in[0], &in[N]})>::type>::type
    {
      return KnowledgeRecord(tags::doubles, &in[0], &in[N]);
    }

    template<size_t N>
    inline KnowledgeRecord knowledge_cast(const char (&in)[N])
    {
      return KnowledgeRecord(tags::string, &in[0], &in[N]);
    }

    template<size_t N>
    inline KnowledgeRecord knowledge_cast(const unsigned char (&in)[N])
    {
      return KnowledgeRecord(tags::binary, &in[0], &in[N]);
    }

    inline KnowledgeRecord &knowledge_cast(KnowledgeRecord &in)
    {
      return in;
    }

    inline const KnowledgeRecord &knowledge_cast(const KnowledgeRecord &in)
    {
      return in;
    }
  }
}

#endif
